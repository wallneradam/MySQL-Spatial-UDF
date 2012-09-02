
#define MSUDF_INTERNAL 1

#include "MySpatialUDF.h"
#include "GeometryUtils.h"


// #define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
	#define DEBUG(...) msudf_debug(__VA_ARGS__);
#else
	#define DEBUG(...)
#endif


my_bool msudf_buffer_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_buffer_init");

	if (args->arg_count < 2 || args->arg_count > 3 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	if (args->arg_count > 2 && args->arg_type[2] != INT_RESULT) {
		strcpy(message,"Wrong type on parameter #3");
		return 1;
	};


	DEBUG("msudf_buffer_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_buffer_init OK2");
	return 0;

}

void msudf_buffer_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_buffer_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_buffer_deinit OK");
}

char *msudf_buffer(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;
	double buffer;
	int quadsegs;

	DEBUG("msudf_buffer");

	wkb = (unsigned char *) (args->args[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	buffer = *((double*) args->args[1]);
	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	if (args->arg_count > 2 && args->arg_type[2] == INT_RESULT) {
		quadsegs = *((int *)args->args[2]);
	} else {
		quadsegs = 8;
	}

	geom2 = GEOSBuffer(geom1,buffer,quadsegs);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);

	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_convexHull_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_convexHull_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	}

	DEBUG("msudf_rconvexHull_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	initid->const_item = 0;
	initid->decimals = 0;
	initid->maybe_null = 1;
	DEBUG("msudf_convexHull_init OK2");
	return 0;

}

void msudf_convexHull_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_convexHull_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_convexHull_deinit OK");
}

char *msudf_convexHull(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_convexHull");

	wkb = (unsigned char *) (args->args[0]);
	DEBUG("geom_length: %d",args->lengths[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	
	if (geom1 == NULL) {
		DEBUG("msudf_convexHull: Invalid geometry.");
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSConvexHull(geom1);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}



void msudf_debug(const char *fmt, ...) {
		va_list ap;

		FILE * fd = fopen("C:\\log.txt","a+" );

		fprintf( fd, "DEBUG: ");

		va_start (ap, fmt);
		vfprintf(fd, fmt, ap);
		va_end(ap);
		fprintf( fd, "\n" );

		fclose(fd);
}

my_bool msudf_difference_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_difference_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_difference_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_difference_init OK2");
	return 0;

}

void msudf_difference_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_difference_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_difference_deinit OK");
}

char *msudf_difference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geomFirst,geomSecond,geomResult;
	int sridFirst,sridSecond
		

	DEBUG("msudf_difference");

	wkb = (unsigned char *) (args->args[0]);
	geomFirst = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	sridFirst = msduf_getInt(wkb);
	wkb = NULL;
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry on first parameter.");
		*is_null = 1;
		return NULL;
	}

	wkb = (unsigned char *) (args->args[1]);
	geomSecond = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[1] - 4);
	sridSecond = msduf_getInt(wkb);
	wkb = NULL;
	if (geomSecond == NULL) {
		strcpy(error,"Invalid geometry on second parameter.");
		*is_null = 1;
		return NULL;
	}

	if (sridFirst != sridSecond) {
		strcpy(error,"First and second geometry has different SRID.");
		*is_null = 1;
		return NULL;
	}

	geomResult = GEOSDifference(geomFirst,geomSecond);

	
	wkb = GEOSGeomToWKB_buf(geomResult,&wkbsize);
	
	if (geomFirst != NULL) GEOSGeom_destroy(geomFirst);
	if (geomSecond != NULL) GEOSGeom_destroy(geomSecond);
	if (geomResult != NULL) GEOSGeom_destroy(geomResult);

	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_intersection_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_intersection_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_intersection_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_intersection_init OK2");
	return 0;

}

void msudf_intersection_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_intersection_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_intersection_deinit OK");
}

char *msudf_intersection(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geomFirst,geomSecond,geomResult;
	int sridFirst,sridSecond
		

	DEBUG("msudf_intersection");

	wkb = (unsigned char *) (args->args[0]);
	geomFirst = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	sridFirst = msduf_getInt(wkb);
	wkb = NULL;
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry on first parameter.");
		*is_null = 1;
		return NULL;
	}

	wkb = (unsigned char *) (args->args[1]);
	geomSecond = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[1] - 4);
	sridSecond = msduf_getInt(wkb);
	wkb = NULL;
	if (geomSecond == NULL) {
		strcpy(error,"Invalid geometry on second parameter.");
		*is_null = 1;
		return NULL;
	}

	if (sridFirst != sridSecond) {
		strcpy(error,"First and second geometry has different SRID.");
		*is_null = 1;
		return NULL;
	}

	geomResult = GEOSIntersection(geomFirst,geomSecond);

	
	wkb = GEOSGeomToWKB_buf(geomResult,&wkbsize);
	
	if (geomFirst != NULL) GEOSGeom_destroy(geomFirst);
	if (geomSecond != NULL) GEOSGeom_destroy(geomSecond);
	if (geomResult != NULL) GEOSGeom_destroy(geomResult);

	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}


my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	transform_params *params;

	DEBUG("msudf_transform_init");

	if (args->arg_count != 4) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	} else if(args->arg_type[2] != INT_RESULT) {
		strcpy(message,"Wrong type on parameter #3");
		return 1;
	} else if(args->arg_type[3] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #3");
		return 1;
	}
 
	DEBUG("msudf_transform_init OK1");
	initid->max_length= 0xFFFFFF;
	params = (transform_params *) malloc(sizeof(transform_params));
	params->pj_src = NULL;
	params->pj_dst = NULL;
	params->srid_src = 0;
	params->srid_dst = 0;
	params->buf = NULL;
	initid->ptr = (char *)params;
	DEBUG("msudf_transform_init OK2");
	return 0;
}

int msduf_getInt(const unsigned char *buf)
{
	int result;
    unsigned char *p = (unsigned char *)&result;

    p[0] = buf[0];
    p[1] = buf[1];
    p[2] = buf[2];
    p[3] = buf[3];

	return result;
}

char *msudf_transform(UDF_INIT *initid, UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb = NULL;
	size_t wkbsize;
	GEOSGeom g1,g2 =NULL;
	
	transform_params *params;
	int srid_src,srid_dst;


	params = (transform_params *) initid->ptr;
	

	DEBUG("msudf_transform");
	wkb = (unsigned char *) (args->args[0]);
	g1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	srid_src = msduf_getInt(wkb);
	wkb = NULL;

	DEBUG("srid_src: %d",srid_src);
	if ((params->pj_src == NULL) || (params->srid_src != srid_src)) {
		params->pj_src = pj_init_plus(args->args[1]);
		params->srid_src = srid_src;
	}

	srid_dst = msduf_getInt((unsigned char *) args->args[2]);
	DEBUG("srid_dst: %d",srid_dst);
	if ((params->pj_dst == NULL) || (params->srid_dst != srid_dst)) {
		params->pj_dst = pj_init_plus(args->args[3]);
		params->srid_dst = srid_dst;
	}

	if (params->pj_src != NULL && params->pj_dst != NULL && g1 != NULL) {
		g2 = gu_transformGeom(g1,params->pj_src,params->pj_dst);
		wkb = GEOSGeomToWKB_buf(g2,&wkbsize);
	} else {
		// initid->ptr = NULL;
		strcpy(error,"Invalid geometry.");
	}

	if (g1 != NULL) GEOSGeom_destroy(g1);
	if (g2 != NULL) GEOSGeom_destroy(g2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (params->buf != NULL) free(params->buf);
		params->buf = (char *) malloc(*length);
		memcpy(params->buf,args->args[2],4);
		memcpy((char *)params->buf + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return params->buf;
	} else {
		*is_null = 1;
		return NULL;
	}

}


void msudf_transform_deinit(UDF_INIT *initid)
{
	transform_params *params;

	if (initid->ptr != NULL) {
		params = (transform_params *) initid->ptr;
		free(params->buf);
		if (params->pj_src != NULL) pj_free(params->pj_src);
		if (params->pj_dst != NULL) pj_free(params->pj_dst);
		free(initid->ptr);
		initid->ptr = NULL;
	}
}

my_bool msudf_symDifference_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_symDifference_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_symDifference_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_symDifference_init OK2");
	return 0;

}

void msudf_symDifference_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_symDifference_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_symDifference_deinit OK");
}

char *msudf_symDifference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geomFirst,geomSecond,geomResult;
	int sridFirst,sridSecond
		

	DEBUG("msudf_symDifference");

	wkb = (unsigned char *) (args->args[0]);
	geomFirst = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	sridFirst = msduf_getInt(wkb);
	wkb = NULL;
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry on first parameter.");
		*is_null = 1;
		return NULL;
	}

	wkb = (unsigned char *) (args->args[1]);
	geomSecond = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[1] - 4);
	sridSecond = msduf_getInt(wkb);
	wkb = NULL;
	if (geomSecond == NULL) {
		strcpy(error,"Invalid geometry on second parameter.");
		*is_null = 1;
		return NULL;
	}

	if (sridFirst != sridSecond) {
		strcpy(error,"First and second geometry has different SRID.");
		*is_null = 1;
		return NULL;
	}

	geomResult = GEOSSymDifference(geomFirst,geomSecond);

	
	wkb = GEOSGeomToWKB_buf(geomResult,&wkbsize);
	
	if (geomFirst != NULL) GEOSGeom_destroy(geomFirst);
	if (geomSecond != NULL) GEOSGeom_destroy(geomSecond);
	if (geomResult != NULL) GEOSGeom_destroy(geomResult);

	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}


my_bool msudf_simplify_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_simplify_init");

	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_simplify_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_simplify_init OK2");
	return 0;

}

void msudf_simplify_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_simplify_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_simplify_deinit OK");
}

char *msudf_simplify(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;
	double tolerance;

	DEBUG("msudf_simplify");

	wkb = (unsigned char *) (args->args[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	tolerance = *((double*) args->args[1]);
	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSSimplify(geom1,tolerance);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_simplifyPreserveTopology_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_ssimplifyPreserveTopology_init");

	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_simplifyPreserveTopology_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_simplifyPreserveTopology_init OK2");
	return 0;

}

void msudf_simplifyPreserveTopology_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_simplifyPreserveTopology_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_simplifyPreserveTopology_deinit OK");
}


char *msudf_simplifyPreserveTopology(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;
	double tolerance;

	DEBUG("msudf_simplifyPreserveTopology");

	wkb = (unsigned char *) (args->args[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	tolerance = *((double*) args->args[1]);
	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSTopologyPreserveSimplify(geom1,tolerance);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_lineMerge_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_lineMerge_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	}

	DEBUG("msudf_lineMerge_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	initid->const_item = 0;
	initid->decimals = 0;
	initid->maybe_null = 1;
	DEBUG("msudf_lineMerge_init OK2");
	return 0;

}

void msudf_lineMerge_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_lineMerge_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_lineMerge_deinit OK");
}


char *msudf_lineMerge(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_lineMerge");

	wkb = (unsigned char *) (args->args[0]);
	DEBUG("geom_length: %d",args->lengths[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	
	if (geom1 == NULL) {
		DEBUG("msudf_lineMerge: Invalid geometry.");
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSLineMerge(geom1);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_reverse_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_reverse_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	}

	DEBUG("msudf_reverse__init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	initid->const_item = 0;
	initid->decimals = 0;
	initid->maybe_null = 1;
	DEBUG("msudf_reverse__init OK2");
	return 0;

}

void msudf_reverse_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_reverse_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_reverse_deinit OK");
}

char *msudf_reverse(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_reverse");

	wkb = (unsigned char *) (args->args[0]);
	DEBUG("geom_length: %d",args->lengths[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	
	if (geom1 == NULL) {
		DEBUG("msudf_reverse: Invalid geometry.");
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = gu_reverseGeom(geom1);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_union_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_union_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_union_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_union_init OK2");
	return 0;

}

void msudf_union_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_union_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_union_deinit OK");
}

char *msudf_union(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geomFirst,geomSecond,geomResult;
	int sridFirst,sridSecond
		

	DEBUG("msudf_union");

	wkb = (unsigned char *) (args->args[0]);
	geomFirst = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	sridFirst = msduf_getInt(wkb);
	wkb = NULL;
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry on first parameter.");
		*is_null = 1;
		return NULL;
	}

	wkb = (unsigned char *) (args->args[1]);
	geomSecond = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[1] - 4);
	sridSecond = msduf_getInt(wkb);
	wkb = NULL;
	if (geomSecond == NULL) {
		strcpy(error,"Invalid geometry on second parameter.");
		*is_null = 1;
		return NULL;
	}

	if (sridFirst != sridSecond) {
		strcpy(error,"First and second geometry has different SRID.");
		*is_null = 1;
		return NULL;
	}

	geomResult = GEOSUnion(geomFirst,geomSecond);

	
	wkb = GEOSGeomToWKB_buf(geomResult,&wkbsize);
	
	if (geomFirst != NULL) GEOSGeom_destroy(geomFirst);
	if (geomSecond != NULL) GEOSGeom_destroy(geomSecond);
	if (geomResult != NULL) GEOSGeom_destroy(geomResult);

	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}


