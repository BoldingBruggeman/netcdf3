/*

This file is part of netcdf-4, a netCDF-like interface for HDF5, or a
HDF5 backend for netCDF, depending on your point of view.

This file handles the nc4 user-defined type functions (i.e. compound
and opaque types).

Copyright 2005, University Corporation for Atmospheric Research. See
the COPYRIGHT file for copying and redistribution conditions.

$Id: nc4type.c,v 1.46 2008/06/05 21:45:17 ed Exp $
*/

#include "nc4internal.h"

/* Find all user-defined types for a location. This finds all
 * user-defined types in a group. */
int 
nc_inq_typeids(int ncid, int *ntypes, int *typeids)
{
   NC_GRP_INFO_T *grp;
   NC_HDF5_FILE_INFO_T *h5;
   NC_TYPE_INFO_T *type;
   int num = 0;
   int retval;

   LOG((2, "nc_inq_typeids: ncid 0x%x", ncid));

   /* Find info for this file and group, and set pointer to each. */
   if ((retval = nc4_find_grp_h5(ncid, &grp, &h5)))
      return retval;

   /* If this is a netCDF-4 file, count types. */
   if (h5 && grp->type)
      for (type = grp->type; type; type = type->next)
      {
	 if (typeids)
	    typeids[num] = type->nc_typeid;
	 num++;
      }

   /* Give the count to the user. */
   if (ntypes)
      *ntypes = num;

   return NC_NOERR;
}


/* This internal function adds a new user defined type to the metadata
 * of a group of an open file. */
static int
add_user_type(int ncid, size_t size, const char *name, nc_type base_typeid,
	      nc_type type_class, nc_type *typeidp)
{
   NC_HDF5_FILE_INFO_T *h5;
   NC_GRP_INFO_T *grp;
   char norm_name[NC_MAX_NAME + 1];
   int retval;

   /* Check and normalize the name. */
   if ((retval = nc4_check_name(name, norm_name)))
      return retval;

   LOG((2, "add_user_type: ncid 0x%x size %d name %s base_typeid %d ", 
	ncid, size, norm_name, base_typeid));

   /* Find group metadata. */
   if ((retval = nc4_find_grp_h5(ncid, &grp, &h5)))
      return retval;

   /* Only netcdf-4 files! */
   if (!h5)
      return NC_ENOTNC4;

   /* Turn on define mode if it is not on. */
   if (!(h5->cmode & NC_INDEF))
      if ((retval = nc_redef(ncid)))
	 return retval;

   /* No size is provided for vlens or enums, get it from the base type. */
   if (type_class == NC_VLEN || type_class == NC_ENUM)
   {
      if ((retval = nc4_get_typelen_mem(grp->file->nc4_info, base_typeid, 0, 
					&size)))
	 return retval;
   }
   else if (size <= 0)
      return NC_EINVAL;

   /* Check that this name is not in use as a var, grp, or type. */
   if ((retval = nc4_check_dup_name(grp, norm_name)))
      return retval;
   
   /* Add to our list of types. */
   if ((retval = nc4_type_list_add(&(grp->type), grp->file->nc4_info->next_typeid, 
				   size, norm_name, type_class, base_typeid)))
      return retval;
   
   /* Return the typeid to the user. */
   if (typeidp)
      *typeidp = grp->file->nc4_info->next_typeid;

   grp->file->nc4_info->next_typeid++;

   return NC_NOERR;
}


/* The sizes of types may vary from platform to platform, but within
 * netCDF files, type sizes are fixed. */
#define NC_CHAR_LEN 1
#define NC_SHORT_LEN 2
#define NC_INT_LEN 4
#define NC_FLOAT_LEN 4
#define NC_DOUBLE_LEN 8
#define NC_INT64_LEN 8

/* Get the name and size of a type. For strings, 0 is returned. For
 * VLEN the base type len is returned. */
int
nc_inq_type(int ncid, nc_type typeid, char *name, size_t *size)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   int retval;
   
   LOG((2, "nc_inq_type: ncid 0x%x typeid %d", ncid, typeid));
   
   /* If this is an atomic type, the answer is easy. */
   switch (typeid)
   {
      case NC_BYTE:
      case NC_CHAR:
      case NC_UBYTE:
	 if (size)
	    *size = NC_CHAR_LEN;
	 return NC_NOERR;
      case NC_SHORT:
      case NC_USHORT:
	 if (size)
	    *size = NC_SHORT_LEN;
	 return NC_NOERR;
      case NC_INT:
      case NC_UINT:
	 if (size)
	    *size = NC_INT_LEN;
	 return NC_NOERR;
      case NC_FLOAT:
	 if (size)
	    *size = NC_FLOAT_LEN;
	 return NC_NOERR;
      case NC_DOUBLE:
	 if (size)
	    *size = NC_DOUBLE_LEN;
	 return NC_NOERR;
      case NC_INT64:
      case NC_UINT64:
	 if (size)
	    *size = NC_INT64_LEN;
	 return NC_NOERR;
      case NC_STRING:
	 if (size)
	    *size = 0; /* can't even guess! */
	 return NC_NOERR;
   }

   /* Find group metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;
   
   /* Find this type. */
   if (!(type = nc4_rec_find_nc_type(grp->file->nc4_info->root_grp, typeid)))
      return NC_EBADTYPE;

   if (name)
      strcpy(name, type->name);
   
   if (size)
      *size = type->size;
   
   return NC_NOERR;
}

/* Create a compound type. */
int
nc_def_compound(int ncid, size_t size, char *name, nc_type *typeidp)
{
   return add_user_type(ncid, size, name, 0, NC_COMPOUND, typeidp);
}

/* Insert a named field into a compound type. */
int
nc_insert_compound(int ncid, nc_type typeid, char *name, size_t offset, 
		   nc_type field_typeid)
{
   return nc_insert_array_compound(ncid, typeid, name, offset, 
				   field_typeid, 0, NULL);
}

/* Insert a named array into a compound type. */
EXTERNL int
nc_insert_array_compound(int ncid, int typeid, char *name, 
			 size_t offset, nc_type field_typeid,
			 int ndims, int *dim_sizesp)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   char norm_name[NC_MAX_NAME + 1];
   int retval;

   LOG((2, "nc_insert_array_compound: ncid 0x%x, typeid %d name %s "
	"offset %d field_typeid %d ndims %d", ncid, typeid, 
	name, offset, field_typeid, ndims));

   /* Check and normalize the name. */
   if ((retval = nc4_check_name(name, norm_name)))
      return retval;

   /* Find file metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;

   /* Find type metadata. */
   if ((retval = nc4_find_type(grp->file->nc4_info, typeid, &type)))
      return retval;

   /* Did the user give us a good compound type typeid? */
   if (!type || type->class != NC_COMPOUND)
      return NC_EBADTYPE;

   /* If this type has already been written to the file, you can't
    * change it. */
   if (type->committed)
      return NC_ETYPDEFINED;

   /* Insert new field into this type's list of fields. */
   if ((retval = nc4_field_list_add(&type->field, type->num_fields, 
				    norm_name, offset, 0, field_typeid,
				    ndims, dim_sizesp)))
      return retval;

   type->num_fields++;
   
   return NC_NOERR;
}

/* Get the name of a compound type. */
int
nc_inq_compound_name(int ncid, nc_type typeid, char *name)
{
   return nc_inq_compound(ncid, typeid, name, NULL, NULL);
}

/* Get the size of a compound type. */
int
nc_inq_compound_size(int ncid, nc_type typeid, size_t *size)
{
   return nc_inq_compound(ncid, typeid, NULL, size, NULL);
}

/* Get the number of fields in this compound type. */
int
nc_inq_compound_nfields(int ncid, nc_type typeid, size_t *nfieldsp)
{
   return nc_inq_compound(ncid, typeid, NULL, NULL, nfieldsp);
}

/* Get the info about a compound type. */
int
nc_inq_compound(int ncid, nc_type typeid, char *name, size_t *sizep, 
		size_t *nfieldsp)
{
   int class, retval;
   
   if ((retval = nc_inq_user_type(ncid, typeid, name, sizep, 
				  NULL, nfieldsp, &class)))
      return retval;

   if (class != NC_COMPOUND)
      return NC_EBADTYPE;
   
   return NC_NOERR;
}

/* Find info about any user defined type. */
int
nc_inq_user_type(int ncid, nc_type typeid, char *name, size_t *size, 
		 nc_type *base_nc_typep, size_t *nfieldsp, int *classp)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   NC_FIELD_INFO_T *field;
   int retval;
   
   LOG((2, "nc_inq_user_type: ncid 0x%x typeid %d", ncid, typeid));

   /* Find group metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;
   
   /* Find this type. */
   if (!(type = nc4_rec_find_nc_type(grp->file->nc4_info->root_grp, typeid)))
      return NC_EBADTYPE;

   /* Count the number of fields. */
   if (nfieldsp)
   {
      *nfieldsp = 0;
      if (type->class == NC_COMPOUND)
	 for (field = type->field; field; field = field->next)
	    (*nfieldsp)++;
      else if (type->class == NC_ENUM)
	 *nfieldsp = type->num_enum_members;
   }

   /* Fill in size and name info, if desired. */
   if (size)
      *size = type->size;
   if (name)
      strcpy(name, type->name);

   /* VLENS and ENUMs have a base type - that is, they type they are
    * arrays of or enums of. */
   if (base_nc_typep)
      *base_nc_typep = type->base_nc_type;

   /* If the user wants it, tell whether this is a compound, opaque,
    * vlen, enum, or string class of type. */
   if (classp)
      *classp = type->class;

   return NC_NOERR;
}

/* Given the ncid, typeid and fieldid, get info about the field. */
int
nc_inq_compound_field(int ncid, nc_type typeid, int fieldid, char *name, 
		      size_t *offsetp, nc_type *field_typeidp, int *ndimsp, 
		      int *dim_sizesp)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   NC_FIELD_INFO_T *field;
   int d, retval;
   
   /* Find file metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;
   
   /* Find this type. */
   if (!(type = nc4_rec_find_nc_type(grp->file->nc4_info->root_grp, typeid)))
      return NC_EBADTYPE;

   /* Find the field. */
   for (field = type->field; field; field = field->next)
      if (field->fieldid == fieldid)
      {
	 if (name)
	    strcpy(name, field->name);
	 if (offsetp)
	    *offsetp = field->offset;
	 if (field_typeidp)
	    *field_typeidp = field->nctype;
	 if (ndimsp)
	    *ndimsp = field->ndims;
	 if (dim_sizesp)
	    for (d = 0; d < field->ndims; d++)
	       dim_sizesp[d] = field->dim_size[d];
	 return NC_NOERR;
      }

   return NC_EBADFIELD;
}

/* Given the ncid, typeid and fieldid, get the name. */
int
nc_inq_compound_fieldname(int ncid, nc_type typeid, 
			  int fieldid, char *name)
{
   return nc_inq_compound_field(ncid, typeid, fieldid, 
				name, NULL, NULL, NULL, NULL);
}

/* Find a netcdf-4 file. THis will return an error if it finds a
 * netcdf-3 file, or a netcdf-4 file with strict nc3 rules. */
static int
find_nc4_file(int ncid, NC_FILE_INFO_T **nc)
{
   
   /* Find file metadata. */
   if (!((*nc) = nc4_find_nc_file(ncid)))
      return NC_EBADID;

   /* Check for netcdf-3 files or netcdf-3 rules. */
   if (!(*nc)->nc4_info)
      return NC_ENOTNC4;
   if ((*nc)->nc4_info->cmode & NC_CLASSIC_MODEL)
      return NC_ESTRICTNC3;

   return NC_NOERR;
}

/* Given the typeid and the name, get the fieldid. */
int
nc_inq_compound_fieldindex(int ncid, nc_type typeid, char *name, int *fieldidp)
{
   NC_FILE_INFO_T *nc;
   NC_TYPE_INFO_T *type;
   NC_FIELD_INFO_T *field;
   char norm_name[NC_MAX_NAME + 1];
   int retval;

   LOG((2, "nc_inq_compound_fieldindex: ncid 0x%x typeid %d name %s",
	ncid, typeid, name));

   /* Find file metadata. */
   if ((retval = find_nc4_file(ncid, &nc)))
      return retval;

   /* Find the type. */
   if ((retval = nc4_find_type(nc->nc4_info, typeid, &type)))
      return retval;

   /* Did the user give us a good compound type typeid? */
   if (!type || type->class != NC_COMPOUND)
      return NC_EBADTYPE;

   /* Normalize name. */
   if ((retval = nc4_normalize_name(name, norm_name)))
      return retval;

   /* Find the field with this name. */
   for (field = type->field; field; field = field->next)
      if (!strcmp(field->name, norm_name))
	 break;

   if (!field)
      return NC_EBADFIELD;

   if (fieldidp)
      *fieldidp = field->fieldid;
   return NC_NOERR;
}

/* Given the typeid and fieldid, get the offset. */
int
nc_inq_compound_fieldoffset(int ncid, nc_type typeid, int fieldid, 
			    size_t *offsetp)
{
   return nc_inq_compound_field(ncid, typeid, fieldid, NULL, 
				offsetp, NULL, NULL, NULL);
}

/* Given the typeid and the fieldid, get the type of that field. */
int
nc_inq_compound_fieldtype(int ncid, nc_type typeid, int fieldid, 
			  nc_type *field_typeidp)
{
   return nc_inq_compound_field(ncid, typeid, fieldid, NULL, NULL, 
				field_typeidp, NULL, NULL);
}

/* Given the typeid and the fieldid, get the number of dimensions for
 * that field (scalars are 0). */
int
nc_inq_compound_fieldndims(int ncid, nc_type typeid, int fieldid, int *ndims)
{
   return nc_inq_compound_field(ncid, typeid, fieldid, NULL, NULL, 
				NULL, ndims, NULL);
}

/* Given the typeid and the fieldid, get the sizes of dimensions for
 * that field. User must have allocated storage for the dim_sizes. */
int
nc_inq_compound_fielddim_sizes(int ncid, nc_type typeid, int fieldid, 
			       int *dim_sizes)
{
   return nc_inq_compound_field(ncid, typeid, fieldid, NULL, NULL, 
				NULL, NULL, dim_sizes);
}

/* Opaque type. */

/* Create an opaque type. Provide a size and a name. */
int
nc_def_opaque(int ncid, size_t datum_size, char *name, 
	      nc_type *typeidp)
{
   return add_user_type(ncid, datum_size, name, 0, NC_OPAQUE, typeidp);
}

/* Get the name and size of an opaque type. */
int
nc_inq_opaque(int ncid, nc_type typeid, char *name, 
	      size_t *sizep)
{
   int class, retval;

   /* Get the info about this type. */
   if ((retval = nc_inq_user_type(ncid, typeid, name, sizep,
				  NULL, NULL, &class)))
      return retval;

   /* If this is not an opaque type, that's an error. */
   if (class != NC_OPAQUE)
      return NC_EBADTYPE;

   return NC_NOERR;
}

/* Write an attribute of opaque type. */
int
nc_put_att_opaque(int ncid, int varid, const char *name,
		  size_t len, void *op)
{
   if (!name)
      return NC_EINVAL;

   LOG((2, "nc_put_att_opaque: ncid 0x%x varid %d name %s len %d", 
	ncid, varid, name, len));

   return NC_NOERR;
}

/* Read an attribute of opaque type. */
int
nc_get_att_opaque(int ncid, int varid, const char *name, 
		  void *ip)
{
   LOG((2, "nc_get_att_opaque:"));
   return NC_NOERR;
}

/* Define a variable length type. */
int
nc_def_vlen(int ncid, char *name, nc_type base_typeid, nc_type *typeidp)
{
   return add_user_type(ncid, 0, name, base_typeid, NC_VLEN, typeidp);
}


/* Find out about a vlen. */
int
nc_inq_vlen(int ncid, nc_type typeid, char *name, size_t *sizep, 
	    nc_type *base_nc_typep)
{
   int class, retval;

   if ((retval = nc_inq_user_type(ncid, typeid, name, sizep, 
				  base_nc_typep, NULL, &class)))
      return retval;

   if (class != NC_VLEN)
      return NC_EBADTYPE;

   return NC_NOERR;
}

/* When you read string type the library will allocate the storage
 * space for the data. This storage space must be freed, so pass the
 * pointer back to this function, when you're done with the data, and
 * it will free the string memory. */
EXTERNL int
nc_free_string(size_t len, char **data)
{
   int i;
   for (i = 0; i < len; i++)
      free(data[i]);
   return NC_NOERR;
}

/* When you read VLEN type the library will actually allocate the
 * storage space for the data. This storage space must be freed, so
 * pass the pointer back to this function, when you're done with the
 * data, and it will free the vlen memory. */
EXTERNL int
nc_free_vlen(nc_vlen_t *vl)
{
   free(vl->p);
   return NC_NOERR;
}

/* Create an enum type. Provide a base type and a name. At the moment
 * only ints are accepted as base types. */
int
nc_def_enum(int ncid, nc_type base_typeid, const char *name, 
	    nc_type *typeidp)
{
   return add_user_type(ncid, 0, name, base_typeid, NC_ENUM, typeidp);
}

/* Get information about an enum type: it's name, base type and the
 * number of members defined. */
int
nc_inq_enum(int ncid, nc_type typeid, char *name, nc_type *base_nc_typep, 
	    size_t *base_sizep, size_t *num_membersp)
{
   int class, retval;
   
   /* Get all the info. */
   if ((retval = nc_inq_user_type(ncid, typeid, name, base_sizep, 
				  base_nc_typep, num_membersp, &class)))
      return retval;

   /* Complain if they are confused about the type. */
   if (class != NC_ENUM)
      return NC_EBADTYPE;
   
   return NC_NOERR;
}

/* Get enum name from enum value. Name size will be <= NC_MAX_NAME. */
int
nc_inq_enum_ident(int ncid, nc_type xtype, long long value, char *identifier)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   NC_ENUM_MEMBER_INFO_T *enum_member;
   long long ll_val;
   int i;
   int retval;

   LOG((3, "nc_inq_enum_ident: xtype %d value %d\n", xtype, value));
   
   /* Find group metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;
   
   /* Find this type. */
   if (!(type = nc4_rec_find_nc_type(grp->file->nc4_info->root_grp, xtype)))
      return NC_EBADTYPE;
   
   /* Complain if they are confused about the type. */
   if (type->class != NC_ENUM)
      return NC_EBADTYPE;
   
   /* Move to the desired enum member in the list. */
   enum_member = type->enum_member;
   for (i = 0; i < type->num_enum_members; i++)
   {
      switch (type->base_nc_type)
      {
	 case NC_BYTE:
	    ll_val = *(char *)enum_member->value;
	    break;
	 case NC_UBYTE:
	    ll_val = *(unsigned char *)enum_member->value;
	    break;
	 case NC_SHORT:
	    ll_val = *(short *)enum_member->value;
	    break;
	 case NC_USHORT:
	    ll_val = *(unsigned short *)enum_member->value;
	    break;
	 case NC_INT:
	    ll_val = *(int *)enum_member->value;
	    break;
	 case NC_UINT:
	    ll_val = *(unsigned int *)enum_member->value;
	    break;
	 case NC_INT64:
	 case NC_UINT64:
	    ll_val = *(long long *)enum_member->value;
	    break;
	 default:
	    return NC_EINVAL;
      }
      LOG((4, "ll_val=%d", ll_val));
      if (ll_val == value)
      {
	 if (identifier)
	    strcpy(identifier, enum_member->name);
	 break;
      }
      else
	 enum_member = enum_member->next;
   }

   /* If we didn't find it, life sucks for us. :-( */
   if (i == type->num_enum_members)
      return NC_EINVAL;

   return NC_NOERR;
}

/* Get information about an enum member: an identifier and
 * value. Identifier size will be <= NC_MAX_NAME. */
int
nc_inq_enum_member(int ncid, nc_type typeid, int idx, char *identifier, 
		   void *value)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   NC_ENUM_MEMBER_INFO_T *enum_member;
   int i;
   int retval;
   
   LOG((2, "nc_inq_enum_member: ncid 0x%x typeid %d", ncid, typeid));

   /* Find group metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;
   
   /* Find this type. */
   if (!(type = nc4_rec_find_nc_type(grp->file->nc4_info->root_grp, typeid)))
      return NC_EBADTYPE;
   
   /* Complain if they are confused about the type. */
   if (type->class != NC_ENUM)
      return NC_EBADTYPE;
   
   /* Check index. */
   if (idx >= type->num_enum_members)
      return NC_EINVAL;
   
   /* Move to the desired enum member in the list. */
   enum_member = type->enum_member;
   for (i = 0; i < idx; i++)
      enum_member = enum_member->next;

   /* Give the people what they want. */
   if (identifier)
      strcpy(identifier, enum_member->name);
   if (value)
      memcpy(value, enum_member->value, type->size);

   return NC_NOERR;
}

/* Insert a identifierd value into an enum type. The value must fit within
 * the size of the enum type, the identifier size must be <= NC_MAX_NAME. */
int
nc_insert_enum(int ncid, nc_type typeid, const char *identifier, 
	       const void *value)
{
   NC_GRP_INFO_T *grp;
   NC_TYPE_INFO_T *type;
   char norm_name[NC_MAX_NAME + 1];
   int retval;

   LOG((2, "nc_insert_enum: ncid 0x%x, typeid %d identifier %s value %d", ncid, 
	typeid, identifier, value));

   /* Check and normalize the name. */
   if ((retval = nc4_check_name(identifier, norm_name)))
      return retval;

   /* Find file metadata. */
   if ((retval = nc4_find_nc4_grp(ncid, &grp)))
      return retval;

   /* Find type metadata. */
   if ((retval = nc4_find_type(grp->file->nc4_info, typeid, &type)))
      return retval;

   /* Did the user give us a good enum typeid? */
   if (!type || type->class != NC_ENUM)
      return NC_EBADTYPE;

   /* If this type has already been written to the file, you can't
    * change it. */
   if (type->committed)
      return NC_ETYPDEFINED;

   /* Insert new field into this type's list of fields. */
   if ((retval = nc4_enum_member_add(&type->enum_member, type->size, 
				     norm_name, value)))
      return retval;

      type->num_enum_members++;
   
   return NC_NOERR;
}

/* Insert one element into an already allocated vlen array element. */
int
nc_put_vlen_element(int ncid, int typeid, void *vlen_element, size_t len, void *data)
{
   nc_vlen_t *tmp = vlen_element;
   tmp->len = len;
   tmp->p = data;
   return NC_NOERR;
}

/* Insert one element into an already allocated vlen array element. */
int
nc_get_vlen_element(int ncid, int typeid, void *vlen_element, size_t *len, void *data)
{
   nc_vlen_t *tmp = vlen_element;
   int type_size = 4;

   *len = tmp->len;
   memcpy(data, tmp->p, tmp->len * type_size);
   return NC_NOERR;
}

