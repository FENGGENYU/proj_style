#include <fstream>
#include <iostream>
#include "model.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
//#include "CommonZ.h"

using namespace std;

#define T(x) (model->triangles[(x)])

//template<typename T>
//extern int length(T *str);
//
//extern void split(const string &s, char delim, vector<string> &elems);

/* _GLMnode: general purpose node */
typedef struct _GLMnode {
	GLuint index;
	GLboolean averaged;
	struct _GLMnode *next;
} GLMnode;


/* glmMax: returns the maximum of two floats */
static GLfloat glmMax(GLfloat a, GLfloat b)
{
	if (b > a)
		return b;
	return a;
}

/* glmAbs: returns the absolute value of a float */
static GLfloat glmAbs(GLfloat f)
{
	if (f < 0)
		return -f;
	return f;
}

/* glmDot: compute the dot product of two vectors
*
* u - array of 3 GLfloats (GLfloat u[3])
* v - array of 3 GLfloats (GLfloat v[3])
*/
static GLfloat glmDot(GLfloat * u, GLfloat * v)
{
	assert(u);
	assert(v);

	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

/* glmCross: compute the cross product of two vectors
*
* u - array of 3 GLfloats (GLfloat u[3])
* v - array of 3 GLfloats (GLfloat v[3])
* n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
*/
static GLvoid glmCross(GLfloat * u, GLfloat * v, GLfloat * n)
{
	assert(u);
	assert(v);
	assert(n);

	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
}

/* glmNormalize: normalize a vector
*
* v - array of 3 GLfloats (GLfloat v[3]) to be normalized
*/
static GLvoid glmNormalize(GLfloat * v)
{
	GLfloat l;

	assert(v);

	l = (GLfloat)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= l;
	v[1] /= l;
	v[2] /= l;
}

/* glmFindGroup: Find a group in the model */
GLMgroup *glmFindGroup(GLMmodel * model, char *name)
{
	GLMgroup *group;

	assert(model);

	group = model->groups;
	while (group) {
		if (!strcmp(name, group->name))
			break;
		group = group->next;
	}

	return group;
}

/* glmAddGroup: Add a group to the model */
GLMgroup *glmAddGroup(GLMmodel * model, char *name)
{
	GLMgroup *group;

	group = glmFindGroup(model, name);
	if (!group) {
		group = new GLMgroup;
		//group = (GLMgroup *)malloc(sizeof(GLMgroup));
		group->name = _strdup(name);
		group->material = 0;
		group->numtriangles = 0;
		group->triangles = NULL;//empty group
		group->next = model->groups;
		model->groups = group;
		model->numgroups++;
	}

	return group;
}

/* glmFindGroup: Find a material in the model */
GLuint glmFindMaterial(GLMmodel * model, char *name)
{
	GLuint i;

	/* XXX doing a linear search on a string key'd list is pretty lame,
	but it works and is fast enough for now. */
	for (i = 0; i < model->nummaterials; i++) {
		if (!strcmp(model->materials[i].name, name))
			goto found;
	}

	/* didn't find the name, so print a warning and return the default
	material (0). */
	//printf("", name);
	//cout << "glmFindMaterial():  can't find material " << name << endl;
	i = 0;

found:
	return i;
}


/* glmDirName: return the directory given a path
*
* path - filesystem path
*
* NOTE: the return value should be free'd.
*/
static char *glmDirName(char *path)
{
	char *dir;
	char *s;

	dir = _strdup(path);

	s = strrchr(dir, '\\');
	if (s)
		s[1] = '\0';
	else
		dir[0] = '\0';

	return dir;
}


/* glmReadMTL: read a wavefront material library file
*
* model - properly initialized GLMmodel structure
* name  - name of the material library
*/

static GLvoid glmReadMTL(GLMmodel * model, char *name)
{

	FILE *file;
	char *dir;
	char *filename;
	char buf[128];
	GLuint nummaterials, i;

	dir = glmDirName(model->pathname);
	filename =
		(char *)malloc(sizeof(char)* (strlen(dir) + strlen(name) + 1));
	strcpy(filename, dir);
	strcat(filename, name);
	free(dir);

	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr,
			"glmReadMTL() failed: can't open material file \"%s\".\n",
			filename);
		system("PAUSE");
		exit(1);
	}
	free(filename);

	/* count the number of materials in the file */
	nummaterials = 1;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':		/* comment */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'n':		/* newmtl */
			fgets(buf, sizeof(buf), file);
			nummaterials++;
			sscanf(buf, "%s %s", buf, buf);
			break;
		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	rewind(file);

	model->materials =
		(GLMmaterial *)malloc(sizeof(GLMmaterial)* nummaterials);
	model->nummaterials = nummaterials;

	/* set the default material */
	for (i = 0; i < nummaterials; i++) {
		model->materials[i].name = NULL;
		model->materials[i].shininess = 65.0;
		model->materials[i].diffuse[0] = 0.8;
		model->materials[i].diffuse[1] = 0.8;
		model->materials[i].diffuse[2] = 0.8;
		model->materials[i].diffuse[3] = 1.0;
		model->materials[i].ambient[0] = 0.2;
		model->materials[i].ambient[1] = 0.2;
		model->materials[i].ambient[2] = 0.2;
		model->materials[i].ambient[3] = 1.0;
		model->materials[i].specular[0] = 0.0;
		model->materials[i].specular[1] = 0.0;
		model->materials[i].specular[2] = 0.0;
		model->materials[i].specular[3] = 1.0;
	}
	model->materials[0].name = _strdup("default");

	/* now, read in the data */
	nummaterials = 0;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':		/* comment */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'n':		/* newmtl */
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s %s", buf, buf);
			nummaterials++;
			model->materials[nummaterials].name = _strdup(buf);
			break;
		case 'N':
			fscanf(file, "%f", &model->materials[nummaterials].shininess);
			/* wavefront shininess is from [0, 1000], so scale for OpenGL */
			model->materials[nummaterials].shininess /= 1000.0;
			model->materials[nummaterials].shininess *= 128.0;
			break;
		case 'K':
			switch (buf[1]) {
			case 'd':
				fscanf(file, "%f %f %f",
					&model->materials[nummaterials].diffuse[0],
					&model->materials[nummaterials].diffuse[1],
					&model->materials[nummaterials].diffuse[2]);
				break;
			case 's':
				fscanf(file, "%f %f %f",
					&model->materials[nummaterials].specular[0],
					&model->materials[nummaterials].specular[1],
					&model->materials[nummaterials].specular[2]);
				break;
			case 'a':
				fscanf(file, "%f %f %f",
					&model->materials[nummaterials].ambient[0],
					&model->materials[nummaterials].ambient[1],
					&model->materials[nummaterials].ambient[2]);
				break;
			default:
				/* eat up rest of line */
				fgets(buf, sizeof(buf), file);
				break;
			}
			break;
		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	fclose(file);
}

/* glmWriteMTL: write a wavefront material library file
*
* model   - properly initialized GLMmodel structure
* modelpath  - pathname of the model being written
* mtllibname - name of the material library to be written
*/
static GLvoid
glmWriteMTL(GLMmodel * model, char *modelpath, char *mtllibname)
{
	FILE *file;
	char *dir;
	char *filename;
	GLMmaterial *material;
	GLuint i;

	dir = glmDirName(modelpath);
	filename =
		(char *)malloc(sizeof(char)* (strlen(dir) + strlen(mtllibname)));
	strcpy(filename, dir);
	strcat(filename, mtllibname);
	free(dir);

	/* open the file */
	file = fopen(filename, "w");
	if (!file) {
		fprintf(stderr, "glmWriteMTL() failed: can't open file \"%s\".\n",
			filename);
		exit(1);
	}
//	free(filename);

	/* spit out a header */
	fprintf(file, "#  \n");
	fprintf(file, "#  Wavefront MTL generated by GLM library\n");
	fprintf(file, "#  \n");
	fprintf(file, "#  GLM library\n");
	fprintf(file, "#  Nate Robins\n");
	fprintf(file, "#  ndr@pobox.com\n");
	fprintf(file, "#  http://www.pobox.com/~ndr\n");
	fprintf(file, "#  \n\n");

	for (i = 0; i < model->nummaterials; i++) {
		material = &model->materials[i];
		fprintf(file, "newmtl %s\n", material->name);
		fprintf(file, "Ka %f %f %f\n",
			material->ambient[0], material->ambient[1],
			material->ambient[2]);
		fprintf(file, "Kd %f %f %f\n", material->diffuse[0],
			material->diffuse[1], material->diffuse[2]);
		fprintf(file, "Ks %f %f %f\n", material->specular[0],
			material->specular[1], material->specular[2]);
		fprintf(file, "Ns %f\n", material->shininess / 128.0 * 1000.0);
		fprintf(file, "\n");
	}

	fclose(file);
}

GLvoid glmTranslate(GLMmodel *model)
{
	model->position[0] /= 150;
	model->position[1] /= 150;
	model->position[2] /= 150;

	for (int i = 1; i <= model->numvertices; i++) {

		model->vertices[3 * i + 0] += model->position[0];
		model->vertices[3 * i + 1] += model->position[2];
		model->vertices[3 * i + 2] += model->position[1];

	}
}


/* glmFirstPass: first pass at a Wavefront OBJ file that gets all the
* statistics of the model (such as #vertices, #normals, etc)
*
* model - properly initialized GLMmodel structure
* file  - (fopen'd) file descriptor
*/
static GLvoid glmFirstPass(GLMmodel * model, FILE * file)
{
	GLuint numvertices;		/* number of vertices in model */
	GLuint numnormals;		/* number of normals in model */
	GLuint numtexcoords;	/* number of texcoords in model */
	GLuint numtriangles;	/* number of triangles in model */
	GLMgroup *group;		/* current group */
	int v, n, t;
	char buf[1024];

	/* make a default group */
	group = glmAddGroup(model, "default");

	numvertices = numnormals = numtexcoords = numtriangles = 0;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':		/* comment */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'v':		/* v, vn, vt */
			switch (buf[1]) {
			case '\0':		/* vertex */
				/* eat up rest of line */
				fgets(buf, sizeof(buf), file);
				numvertices++;
				break;
			case 'n':		/* normal */
				/* eat up rest of line */
				fgets(buf, sizeof(buf), file);
				numnormals++;
				break;
			case 't':		/* texcoord */
				/* eat up rest of line */
				fgets(buf, sizeof(buf), file);
				numtexcoords++;
				break;
			default:
				printf("glmFirstPass(): Unknown token \"%s\".\n", buf);
				exit(1);
				break;
			}
			break;
		case 'm':
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s %s", buf, buf);
			model->mtllibname = _strdup(buf);
//			glmReadMTL(model, buf);
			break;
		case 'u':
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'g':		/* group */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
			sscanf(buf, "%s", buf);
#else
			buf[strlen(buf) - 1] = '\0';	/* nuke '\n' */
#endif
			group = glmAddGroup(model, buf);
			break;
		case 'f':		/* face */
			v = n = t = 0;
			fscanf(file, "%s", buf);
			/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
			if (strstr(buf, "//")) {
				/* v//n */
				sscanf(buf, "%d//%d", &v, &n);
				fscanf(file, "%d//%d", &v, &n);
				fscanf(file, "%d//%d", &v, &n);
				numtriangles++;
				group->numtriangles++;
				while (fscanf(file, "%d//%d", &v, &n) > 0) {
					numtriangles++;
					group->numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
				/* v/t/n */
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				numtriangles++;
				group->numtriangles++;
				while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
					numtriangles++;
					group->numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
				/* v/t */
				fscanf(file, "%d/%d", &v, &t);
				fscanf(file, "%d/%d", &v, &t);
				numtriangles++;
				group->numtriangles++;
				while (fscanf(file, "%d/%d", &v, &t) > 0) {
					numtriangles++;
					group->numtriangles++;
				}
			}
			else {
				/* v */
				fscanf(file, "%d", &v);
				fscanf(file, "%d", &v);
				numtriangles++;
				group->numtriangles++;
				while (fscanf(file, "%d", &v) > 0) {
					numtriangles++;
					group->numtriangles++;
				}
			}
			break;

		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	/* set the stats in the model structure */
	model->numvertices = numvertices;
	model->numnormals = numnormals;
	model->numtexcoords = numtexcoords;
	model->numtriangles = numtriangles;

	/* allocate memory for the triangles in each group */
	group = model->groups;
	while (group) {
		group->triangles =
			(GLuint *)malloc(sizeof(GLuint)* group->numtriangles);
		group->numtriangles = 0;
		group = group->next;
	}
}

/* glmSecondPass: second pass at a Wavefront OBJ file that gets all
* the data.
*
* model - properly initialized GLMmodel structure
* file  - (fopen'd) file descriptor
*/
static GLvoid glmSecondPass(GLMmodel * model, FILE * file)
{
	GLuint numvertices;		/* number of vertices in model */
	GLuint numnormals;		/* number of normals in model */
	GLuint numtexcoords;	/* number of texcoords in model */
	GLuint numtriangles;	/* number of triangles in model */
	GLfloat *vertices;		/* array of vertices  */
	GLfloat *normals;		/* array of normals */
	GLfloat *texcoords;		/* array of texture coordinates */
	GLMgroup *group;		/* current group pointer */
	GLuint material;		/* current material */
	int v, n, t;
	char buf[1024];

	/* set the pointer shortcuts */
	vertices = model->vertices;
	normals = model->normals;
	texcoords = model->texcoords;
	group = model->groups;

	/* on the second pass through the file, read all the data into the
	allocated arrays */
	numvertices = numnormals = numtexcoords = 1;
	numtriangles = 0;
	material = 0;
	while (fscanf(file, "%s", buf) != EOF) {
		switch (buf[0]) {
		case '#':		/* comment */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		case 'v':		/* v, vn, vt */
			switch (buf[1]) {
			case '\0':		/* vertex */
				fscanf(file, "%f %f %f",
					&vertices[3 * numvertices + 0],
					&vertices[3 * numvertices + 1],
					&vertices[3 * numvertices + 2]);
				numvertices++;
				break;
			case 'n':		/* normal */
				fscanf(file, "%f %f %f",
					&normals[3 * numnormals + 0],
					&normals[3 * numnormals + 1],
					&normals[3 * numnormals + 2]);
				numnormals++;
				break;
			case 't':		/* texcoord */
				fscanf(file, "%f %f",
					&texcoords[2 * numtexcoords + 0],
					&texcoords[2 * numtexcoords + 1]);
				numtexcoords++;
				break;
			}
			break;
		case 'u':
			fgets(buf, sizeof(buf), file);
			sscanf(buf, "%s %s", buf, buf);
//			group->material = material = glmFindMaterial(model, buf);
			break;
		case 'g':		/* group */
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
			sscanf(buf, "%s", buf);
#else
			buf[strlen(buf) - 1] = '\0';	/* nuke '\n' */
#endif
			group = glmFindGroup(model, buf);
			group->material = material;
			break;
		case 'f':		/* face */
			v = n = t = 0;
			fscanf(file, "%s", buf);
			/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
			if (strstr(buf, "//")) {
				/* v//n */
				sscanf(buf, "%d//%d", &v, &n);
				T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
				T(numtriangles).nindices[0] = n < 0 ? n + numnormals : n;
				fscanf(file, "%d//%d", &v, &n);
				T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
				T(numtriangles).nindices[1] = n < 0 ? n + numnormals : n;
				fscanf(file, "%d//%d", &v, &n);
				T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
				T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d//%d", &v, &n) > 0) {
					T(numtriangles).vindices[0] =
						T(numtriangles - 1).vindices[0];
					T(numtriangles).nindices[0] =
						T(numtriangles - 1).nindices[0];
					T(numtriangles).vindices[1] =
						T(numtriangles - 1).vindices[2];
					T(numtriangles).nindices[1] =
						T(numtriangles - 1).nindices[2];
					T(numtriangles).vindices[2] =
						v < 0 ? v + numvertices : v;
					T(numtriangles).nindices[2] =
						n < 0 ? n + numnormals : n;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
				/* v/t/n */
				T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
				T(numtriangles).tindices[0] = t < 0 ? t + numtexcoords : t;
				T(numtriangles).nindices[0] = n < 0 ? n + numnormals : n;
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
				T(numtriangles).tindices[1] = t < 0 ? t + numtexcoords : t;
				T(numtriangles).nindices[1] = n < 0 ? n + numnormals : n;
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
				T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
				T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
					T(numtriangles).vindices[0] =
						T(numtriangles - 1).vindices[0];
					T(numtriangles).tindices[0] =
						T(numtriangles - 1).tindices[0];
					T(numtriangles).nindices[0] =
						T(numtriangles - 1).nindices[0];
					T(numtriangles).vindices[1] =
						T(numtriangles - 1).vindices[2];
					T(numtriangles).tindices[1] =
						T(numtriangles - 1).tindices[2];
					T(numtriangles).nindices[1] =
						T(numtriangles - 1).nindices[2];
					T(numtriangles).vindices[2] =
						v < 0 ? v + numvertices : v;
					T(numtriangles).tindices[2] =
						t < 0 ? t + numtexcoords : t;
					T(numtriangles).nindices[2] =
						n < 0 ? n + numnormals : n;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
				/* v/t */
				T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
				T(numtriangles).tindices[0] = t < 0 ? t + numtexcoords : t;
				fscanf(file, "%d/%d", &v, &t);
				T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
				T(numtriangles).tindices[1] = t < 0 ? t + numtexcoords : t;
				fscanf(file, "%d/%d", &v, &t);
				T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
				T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d/%d", &v, &t) > 0) {
					T(numtriangles).vindices[0] =
						T(numtriangles - 1).vindices[0];
					T(numtriangles).tindices[0] =
						T(numtriangles - 1).tindices[0];
					T(numtriangles).vindices[1] =
						T(numtriangles - 1).vindices[2];
					T(numtriangles).tindices[1] =
						T(numtriangles - 1).tindices[2];
					T(numtriangles).vindices[2] =
						v < 0 ? v + numvertices : v;
					T(numtriangles).tindices[2] =
						t < 0 ? t + numtexcoords : t;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			else {
				/* v */
				sscanf(buf, "%d", &v);
				T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
				fscanf(file, "%d", &v);
				T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
				fscanf(file, "%d", &v);
				T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
				group->triangles[group->numtriangles++] = numtriangles;
				numtriangles++;
				while (fscanf(file, "%d", &v) > 0) {
					T(numtriangles).vindices[0] =
						T(numtriangles - 1).vindices[0];
					T(numtriangles).vindices[1] =
						T(numtriangles - 1).vindices[2];
					T(numtriangles).vindices[2] =
						v < 0 ? v + numvertices : v;
					group->triangles[group->numtriangles++] = numtriangles;
					numtriangles++;
				}
			}
			break;

		default:
			/* eat up rest of line */
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

#if 0
	/* announce the memory requirements */
	printf(" Memory: %d bytes\n",
		numvertices * 3 * sizeof(GLfloat)+
		numnormals * 3 * sizeof(GLfloat)* (numnormals ? 1 : 0) +
		numtexcoords * 3 * sizeof(GLfloat)* (numtexcoords ? 1 : 0) +
		numtriangles * sizeof(GLMtriangle));
#endif
}


/* public functions */
GLvoid glmRotate(GLMmodel * model)
{
	GLfloat *temp = (GLfloat *)malloc(sizeof(GLfloat)*(model->numvertices+1));
	for (int i = 1; i <= model->numvertices; i++) {

		temp[i] = model->vertices[3 * i + 0];

		model->vertices[3 * i + 0] = model->vertices[3 * i + 1];
		model->vertices[3 * i + 1] = -temp[i];

		//temp[i] = model->vertices[3 * i + 2];

		//model->vertices[3 * i + 2] = -model->vertices[3 * i + 1];
		//model->vertices[3 * i + 1] = temp[i];

	}
	free(temp);
}

/* glmUnitize: "unitize" a model by translating it to the origin and
* scaling it to fit in a unit cube around the origin.   Returns the
* scale factor used.
*
* model - properly initialized GLMmodel structure
*/
GLfloat glmUnitize(GLMmodel * model)
{
	GLuint i;
	GLfloat maxx, minx, maxy, miny, maxz, minz;
	GLfloat cx, cy, cz;

	assert(model);
	assert(model->vertices);

	/* get the max/min */
	maxx = minx = model->vertices[3 + 0];
	maxy = miny = model->vertices[3 + 1];
	maxz = minz = model->vertices[3 + 2];
	for (i = 1; i <= model->numvertices; i++) {
		if (maxx < model->vertices[3 * i + 0])
			maxx = model->vertices[3 * i + 0];
		if (minx > model->vertices[3 * i + 0])
			minx = model->vertices[3 * i + 0];

		if (maxy < model->vertices[3 * i + 1])
			maxy = model->vertices[3 * i + 1];
		if (miny > model->vertices[3 * i + 1])
			miny = model->vertices[3 * i + 1];

		if (maxz < model->vertices[3 * i + 2])
			maxz = model->vertices[3 * i + 2];
		if (minz > model->vertices[3 * i + 2])
			minz = model->vertices[3 * i + 2];
	}

	/* calculate center of the model */
	cx = (maxx + minx) / 2.0;
	cy = (maxy + miny) / 2.0;
	cz = (maxz + minz) / 2.0;

	/* calculate unitizing scale factor */
	
	GLfloat max =0.0;
	maxx = maxx-minx;
	maxy = maxy-miny;
	maxz = maxz-minz;
	/* calculate unitizing scale factor */
	if(abs(maxx) > abs(maxy))
		if(abs(maxy)>abs(maxz))
			max = maxx;
		else if(abs(maxx)>abs(maxz))
			max = maxx;
		else 
			max = maxz;
	else{ 
		if(abs(maxx)>abs(maxz))
			max = maxy;
		else if(abs(maxy)>abs(maxz))
			max = maxy;
		else 
			max = maxz;
	}

	GLfloat	scale = 1.6 / max;
	
	//scale = 50 / maxy;
	

	/* translate around bottom-center then scale */
	for (i = 1; i <= model->numvertices; i++) {
		model->vertices[3 * i + 0] -= cx;
		//model->vertices[3 * i + 1] -= miny;
		model->vertices[3 * i + 1] -= cy;
		model->vertices[3 * i + 2] -= cz;
		model->vertices[3 * i + 0] *= scale;
		model->vertices[3 * i + 1] *= scale;
		model->vertices[3 * i + 2] *= scale;
	}
	

	return scale;
}



/* glmFacetNormals: Generates facet normals for a model (by taking the
* cross product of the two vectors derived from the sides of each
* triangle).  Assumes a counter-clockwise winding.
*
* model - initialized GLMmodel structure
*/
GLvoid glmFacetNormals(GLMmodel * model)
{
	GLuint i;
	GLfloat u[3];
	GLfloat v[3];

	assert(model);
	assert(model->vertices);

	/* clobber any old facetnormals */
	if (model->facetnorms)
		free(model->facetnorms);

	/* allocate memory for the new facet normals */
	model->numfacetnorms = model->numtriangles;
	model->facetnorms = (GLfloat *)malloc(sizeof(GLfloat)*
		3 * (model->numfacetnorms + 1));

	for (i = 0; i < model->numtriangles; i++) {
		model->triangles[i].findex = i + 1;

		u[0] = model->vertices[3 * T(i).vindices[1] + 0] -
			model->vertices[3 * T(i).vindices[0] + 0];
		u[1] = model->vertices[3 * T(i).vindices[1] + 1] -
			model->vertices[3 * T(i).vindices[0] + 1];
		u[2] = model->vertices[3 * T(i).vindices[1] + 2] -
			model->vertices[3 * T(i).vindices[0] + 2];

		v[0] = model->vertices[3 * T(i).vindices[2] + 0] -
			model->vertices[3 * T(i).vindices[0] + 0];
		v[1] = model->vertices[3 * T(i).vindices[2] + 1] -
			model->vertices[3 * T(i).vindices[0] + 1];
		v[2] = model->vertices[3 * T(i).vindices[2] + 2] -
			model->vertices[3 * T(i).vindices[0] + 2];

		glmCross(u, v, &model->facetnorms[3 * (i + 1)]);
		glmNormalize(&model->facetnorms[3 * (i + 1)]);
	}
}

/* glmVertexNormals: Generates smooth vertex normals for a model.
* First builds a list of all the triangles each vertex is in.   Then
* loops through each vertex in the the list averaging all the facet
* normals of the triangles each vertex is in.   Finally, sets the
* normal index in the triangle for the vertex to the generated smooth
* normal.   If the dot product of a facet normal and the facet normal
* associated with the first triangle in the list of triangles the
* current vertex is in is greater than the cosine of the angle
* parameter to the function, that facet normal is not added into the
* average normal calculation and the corresponding vertex is given
* the facet normal.  This tends to preserve hard edges.  The angle to
* use depends on the model, but 90 degrees is usually a good start.
*
* model - initialized GLMmodel structure
* angle - maximum angle (in degrees) to smooth across
*/
GLvoid glmVertexNormals(GLMmodel * model, GLfloat angle)
{
	GLMnode *node;
	GLMnode *tail;
	GLMnode **members;
	GLfloat *normals;
	GLuint numnormals;
	GLfloat average[3];
	GLfloat dot, cos_angle;
	GLuint i, avg;

	assert(model);
	assert(model->facetnorms);

	/* calculate the cosine of the angle (in degrees) */
	cos_angle = cos(angle * M_PI / 180.0);

	/* nuke any previous normals */
	if (model->normals)
		free(model->normals);

	/* allocate space for new normals */
	model->numnormals = model->numtriangles * 3;	/* 3 normals per triangle */
	model->normals =
		(GLfloat *)malloc(sizeof(GLfloat)* 3 * (model->numnormals + 1));

	/* allocate a structure that will hold a linked list of triangle
	indices for each vertex */
	members =
		(GLMnode **)malloc(sizeof(GLMnode *)* (model->numvertices + 1));
	for (i = 1; i <= model->numvertices; i++)
		members[i] = NULL;

	/* for every triangle, create a node for each vertex in it */
	for (i = 0; i < model->numtriangles; i++) {
		node = (GLMnode *)malloc(sizeof(GLMnode));
		node->index = i;
		node->next = members[T(i).vindices[0]];
		members[T(i).vindices[0]] = node;

		node = (GLMnode *)malloc(sizeof(GLMnode));
		node->index = i;
		node->next = members[T(i).vindices[1]];
		members[T(i).vindices[1]] = node;

		node = (GLMnode *)malloc(sizeof(GLMnode));
		node->index = i;
		node->next = members[T(i).vindices[2]];
		members[T(i).vindices[2]] = node;
	}

	/* calculate the average normal for each vertex */
	numnormals = 1;
	for (i = 1; i <= model->numvertices; i++) {
		/* calculate an average normal for this vertex by averaging the
		facet normal of every triangle this vertex is in */
		node = members[i];
		if (!node)
			fprintf(stderr, "glmVertexNormals(): vertex w/o a triangle\n");
		average[0] = 0.0;
		average[1] = 0.0;
		average[2] = 0.0;
		avg = 0;
		while (node) {
			/* only average if the dot product of the angle between the two
			facet normals is greater than the cosine of the threshold
			angle -- or, said another way, the angle between the two
			facet normals is less than (or equal to) the threshold angle */
			dot = glmDot(&model->facetnorms[3 * T(node->index).findex],
				&model->facetnorms[3 *
				T(members[i]->index).findex]);
			if (dot > cos_angle) {
				node->averaged = GL_TRUE;
				average[0] +=
					model->facetnorms[3 * T(node->index).findex + 0];
				average[1] +=
					model->facetnorms[3 * T(node->index).findex + 1];
				average[2] +=
					model->facetnorms[3 * T(node->index).findex + 2];
				avg = 1;	/* we averaged at least one normal! */
			}
			else {
				node->averaged = GL_FALSE;
			}
			node = node->next;
		}

		if (avg) {
			/* normalize the averaged normal */
			glmNormalize(average);

			/* add the normal to the vertex normals list */
			model->normals[3 * numnormals + 0] = average[0];
			model->normals[3 * numnormals + 1] = average[1];
			model->normals[3 * numnormals + 2] = average[2];
			avg = numnormals;
			numnormals++;
		}

		/* set the normal of this vertex in each triangle it is in */
		node = members[i];
		while (node) {
			if (node->averaged) {
				/* if this node was averaged, use the average normal */
				if (T(node->index).vindices[0] == i)
					T(node->index).nindices[0] = avg;
				else if (T(node->index).vindices[1] == i)
					T(node->index).nindices[1] = avg;
				else if (T(node->index).vindices[2] == i)
					T(node->index).nindices[2] = avg;
			}
			else {
				/* if this node wasn't averaged, use the facet normal */
				model->normals[3 * numnormals + 0] =
					model->facetnorms[3 * T(node->index).findex + 0];
				model->normals[3 * numnormals + 1] =
					model->facetnorms[3 * T(node->index).findex + 1];
				model->normals[3 * numnormals + 2] =
					model->facetnorms[3 * T(node->index).findex + 2];
				if (T(node->index).vindices[0] == i)
					T(node->index).nindices[0] = numnormals;
				else if (T(node->index).vindices[1] == i)
					T(node->index).nindices[1] = numnormals;
				else if (T(node->index).vindices[2] == i)
					T(node->index).nindices[2] = numnormals;
				numnormals++;
			}
			node = node->next;
		}
	}

	model->numnormals = numnormals - 1;

	/* free the member information */
	for (i = 1; i <= model->numvertices; i++) {
		node = members[i];
		while (node) {
			tail = node;
			node = node->next;
			free(tail);
		}
	}
	free(members);

	/* pack the normals array (we previously allocated the maximum
	number of normals that could possibly be created (numtriangles *
	3), so get rid of some of them (usually alot unless none of the
	facet normals were averaged)) */
	normals = model->normals;
	model->normals =
		(GLfloat *)malloc(sizeof(GLfloat)* 3 * (model->numnormals + 1));
	for (i = 1; i <= model->numnormals; i++) {
		model->normals[3 * i + 0] = normals[3 * i + 0];
		model->normals[3 * i + 1] = normals[3 * i + 1];
		model->normals[3 * i + 2] = normals[3 * i + 2];
	}
	free(normals);
}

/* glmDelete: Deletes a GLMmodel structure.
*
* model - initialized GLMmodel structure
*/
GLvoid glmDelete(GLMmodel * model)
{
	GLMgroup *group;
	GLuint i;

	assert(model);

	if (model->pathname)
		free(model->pathname);
	if (model->mtllibname)
		free(model->mtllibname);
	if (model->vertices)
		free(model->vertices);
	if (model->normals)
		free(model->normals);
	if (model->texcoords)
		free(model->texcoords);
	if (model->facetnorms)
		free(model->facetnorms);
	if (model->triangles)
		free(model->triangles);
	if (model->materials) {
		for (i = 0; i < model->nummaterials; i++)
			free(model->materials[i].name);
	}

	if(model->seeds)
		free(model->seeds);
	/*if(model->iscolored)
		free(model->iscolored);*/
	if(model->isstyle)
		delete(model->isstyle);

	free(model->materials);
	while (model->groups) {
		group = model->groups;
		model->groups = model->groups->next;
		free(group->name);
		free(group->triangles);
		free(group);
	}

	free(model);
}

/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
* Returns a pointer to the created object which should be free'd with
* glmDelete().
*
* filename - name of the file containing the Wavefront .OBJ format data.
*/
GLMmodel *glmReadOBJ(char *filename)
{
	GLMmodel *model;
	FILE *file;

	/* open the file */
	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr,
			"glmReadOBJ() failed: can't open data file \"%s\".\n",
			filename);
		exit(1);
	}

	/* allocate a new model */
	model = (GLMmodel *)malloc(sizeof(GLMmodel));
	model->pathname = _strdup(filename);
	model->seed_current = 0;

	model->mtllibname = NULL;
	model->numvertices = 0;
	model->vertices = NULL;
	model->numnormals = 0;
	model->normals = NULL;
	model->numtexcoords = 0;
	model->texcoords = NULL;
	model->numfacetnorms = 0;
	model->facetnorms = NULL;
	model->numtriangles = 0;
	model->triangles = NULL;
	model->nummaterials = 0;
	model->materials = NULL;
	model->numgroups = 0;
	model->groups = NULL;
	model->position[0] = 0.0;
	model->position[1] = 0.0;
	model->position[2] = 0.0;

	//model->iscolored=NULL;
	model->isstyle=NULL;
	model->seeds=NULL;
	/* make a first pass through the file to get a count of the number
	of vertices, normals, texcoords & triangles */
	glmFirstPass(model, file);

	/* allocate memory */
	model->vertices = (GLfloat *)malloc(sizeof(GLfloat)*
		3 * (model->numvertices + 1));

	model->triangles = (GLMtriangle *)malloc(sizeof(GLMtriangle)*
		model->numtriangles);
	if (model->numnormals) {
		model->normals = (GLfloat *)malloc(sizeof(GLfloat)*
			3 * (model->numnormals + 1));
	}
	if (model->numtexcoords) {
		model->texcoords = (GLfloat *)malloc(sizeof(GLfloat)*
			2 * (model->numtexcoords +
			1));
	}

	/* rewind to beginning of file and read in the data this pass */
	rewind(file);

	glmSecondPass(model, file);

	/* close the file */
	fclose(file);

	glmFacetNormals(model);
	//glmVertexNormals(model, 90.0);
	glmUnitize(model);
	//glmCalBBox(model);

//	GLfloat scale_factor;
//	scale_factor = glmUnitize(model, scale);
//	printf("now scale is %f \n", scale_factor);

	//glmRotate(model);
	//glmWriteOBJ(model, model->pathname, GLM_NONE);

	return model;
}

/* glmWriteOBJ: Writes a model description in Wavefront .OBJ format to
* a file.
*
* model - initialized GLMmodel structure
* filename - name of the file to write the Wavefront .OBJ format data to
* mode  - a bitwise or of values describing what is written to the file
*             GLM_NONE     -  render with only vertices
*             GLM_FLAT     -  render with facet normals
*             GLM_SMOOTH   -  render with vertex normals
*             GLM_TEXTURE  -  render with texture coords
*             GLM_COLOR    -  render with colors (color material)
*             GLM_MATERIAL -  render with materials
*             GLM_COLOR and GLM_MATERIAL should not both be specified.
*             GLM_FLAT and GLM_SMOOTH should not both be specified.
*/
GLvoid glmWriteOBJ(GLMmodel * model, char *filename, GLuint mode)
{
	GLuint i;
	FILE *file;
	GLMgroup *group;

	assert(model);

	/* do a bit of warning */
	if (mode & GLM_FLAT && !model->facetnorms) {
		printf("glmWriteOBJ() warning: flat normal output requested "
			"with no facet normals defined.\n");
		mode &= ~GLM_FLAT;
	}
	if (mode & GLM_SMOOTH && !model->normals) {
		printf("glmWriteOBJ() warning: smooth normal output requested "
			"with no normals defined.\n");
		mode &= ~GLM_SMOOTH;
	}
	if (mode & GLM_TEXTURE && !model->texcoords) {
		printf
			("glmWriteOBJ() warning: texture coordinate output requested "
			"with no texture coordinates defined.\n");
		mode &= ~GLM_TEXTURE;
	}
	if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
		printf("glmWriteOBJ() warning: flat normal output requested "
			"and smooth normal output requested (using smooth).\n");
		mode &= ~GLM_FLAT;
	}
	if (mode & GLM_COLOR && !model->materials) {
		printf("glmWriteOBJ() warning: color output requested "
			"with no colors (materials) defined.\n");
		mode &= ~GLM_COLOR;
	}
	if (mode & GLM_MATERIAL && !model->materials) {
		printf("glmWriteOBJ() warning: material output requested "
			"with no materials defined.\n");
		mode &= ~GLM_MATERIAL;
	}
	if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
		printf
			("glmWriteOBJ() warning: color and material output requested "
			"outputting only materials.\n");
		mode &= ~GLM_COLOR;
	}


	/* open the file */
	file = fopen(filename, "w");
	if (!file) {
		fprintf(stderr,
			"glmWriteOBJ() failed: can't open file \"%s\" to write.\n",
			filename);
		exit(1);
	}

	/* spit out a header */
	fprintf(file, "#  \n");
	fprintf(file, "#  Wavefront OBJ generated by GLM library\n");
	fprintf(file, "#  \n");
	fprintf(file, "#  GLM library\n");
	fprintf(file, "#  Nate Robins\n");
	fprintf(file, "#  ndr@pobox.com\n");
	fprintf(file, "#  http://www.pobox.com/~ndr\n");
	fprintf(file, "#  \n");

	///*if (mode & GLM_MATERIAL && model->mtllibname) */{
	//	fprintf(file, "\nmtllib %s\n\n", model->mtllibname);
	//	glmWriteMTL(model, filename, model->mtllibname);
	//}

	fprintf(file, "\nmtllib %s\n\n","abc.mtl");
//	glmWriteMTL(model, filename, model->mtllibname);

	/* spit out the vertices */
	fprintf(file, "\n");
	fprintf(file, "# %d vertices\n", model->numvertices);
	for (i = 1; i <= model->numvertices; i++) {
		fprintf(file, "v %f %f %f\n",
			model->vertices[3 * i + 0],
			model->vertices[3 * i + 1], model->vertices[3 * i + 2]);
	}

	/* spit out the smooth/flat normals */
	if (mode & GLM_SMOOTH) {
		fprintf(file, "\n");
		fprintf(file, "# %d normals\n", model->numnormals);
		for (i = 1; i <= model->numnormals; i++) {
			fprintf(file, "vn %f %f %f\n",
				model->normals[3 * i + 0],
				model->normals[3 * i + 1], model->normals[3 * i + 2]);
		}
	}
	else if (mode & GLM_FLAT) {
		fprintf(file, "\n");
		fprintf(file, "# %d normals\n", model->numfacetnorms);
		for (i = 1; i <= model->numfacetnorms; i++) {
			fprintf(file, "vn %f %f %f\n",
				model->facetnorms[3 * i + 0],
				model->facetnorms[3 * i + 1],
				model->facetnorms[3 * i + 2]);
		}
	}

	/* spit out the texture coordinates */
	if (mode & GLM_TEXTURE) {
		fprintf(file, "\n");
		fprintf(file, "# %d texcoords\n", model->numtexcoords);
		for (i = 1; i <= model->numtexcoords; i++) {
			fprintf(file, "vt %f %f\n",
				model->texcoords[2 * i + 0],
				model->texcoords[2 * i + 1]);
		}
	}

	fprintf(file, "\n");
	fprintf(file, "# %d groups\n", model->numgroups);
	fprintf(file, "# %d faces (triangles)\n", model->numtriangles);
	fprintf(file, "\n");

	group = model->groups;
	while (group) {
		fprintf(file, "g %s\n", group->name);
		
			fprintf(file, "usemtl %s\n",COLOR);
		for (i = 0; i < group->numtriangles; i++) {
			if (mode & GLM_SMOOTH && mode & GLM_TEXTURE) {
				fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					T(group->triangles[i]).vindices[0],
					T(group->triangles[i]).tindices[0],
					T(group->triangles[i]).nindices[0],
					T(group->triangles[i]).vindices[1],
					T(group->triangles[i]).tindices[1],
					T(group->triangles[i]).nindices[1],
					T(group->triangles[i]).vindices[2],
					T(group->triangles[i]).tindices[2],
					T(group->triangles[i]).nindices[2]);
			}
			else if (mode & GLM_FLAT && mode & GLM_TEXTURE) {
				fprintf(file, "f %d/%d %d/%d %d/%d\n",
					T(group->triangles[i]).vindices[0],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vindices[1],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vindices[2],
					T(group->triangles[i]).findex);
			}
			else if (mode & GLM_TEXTURE) {
				fprintf(file, "f %d/%d %d/%d %d/%d\n",
					T(group->triangles[i]).vindices[0],
					T(group->triangles[i]).tindices[0],
					T(group->triangles[i]).vindices[1],
					T(group->triangles[i]).tindices[1],
					T(group->triangles[i]).vindices[2],
					T(group->triangles[i]).tindices[2]);
			}
			else if (mode & GLM_SMOOTH) {
				fprintf(file, "f %d//%d %d//%d %d//%d\n",
					T(group->triangles[i]).vindices[0],
					T(group->triangles[i]).nindices[0],
					T(group->triangles[i]).vindices[1],
					T(group->triangles[i]).nindices[1],
					T(group->triangles[i]).vindices[2],
					T(group->triangles[i]).nindices[2]);
			}
			else if (mode & GLM_FLAT) {
				fprintf(file, "f %d//%d %d//%d %d//%d\n",
					T(group->triangles[i]).vindices[0],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vindices[1],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vindices[2],
					T(group->triangles[i]).findex);
			}
			else {
				fprintf(file, "f %d %d %d\n",
					T(group->triangles[i]).vindices[0],
					T(group->triangles[i]).vindices[1],
					T(group->triangles[i]).vindices[2]);
			}
		}
		fprintf(file, "\n");
		group = group->next;
	}

	fclose(file);
}

/* glmCalBBox: Calculates the AABB of a model
*
* model      - initialized GLMmodel structure
*/
GLvoid glmCalBBox(GLMmodel * model)
{
	GLuint i;
	GLfloat maxx, minx, maxy, miny, maxz, minz;

	assert(model);
	assert(model->vertices);

	/* get the max/mins */
	maxx = minx = model->vertices[3 + 0];
	maxy = miny = model->vertices[3 + 1];
	maxz = minz = model->vertices[3 + 2];
	for (i = 1; i <= model->numvertices; i++) {
		if (maxx < model->vertices[3 * i + 0])
			maxx = model->vertices[3 * i + 0];
		if (minx > model->vertices[3 * i + 0])
			minx = model->vertices[3 * i + 0];

		if (maxy < model->vertices[3 * i + 1])
			maxy = model->vertices[3 * i + 1];
		if (miny > model->vertices[3 * i + 1])
			miny = model->vertices[3 * i + 1];

		if (maxz < model->vertices[3 * i + 2])
			maxz = model->vertices[3 * i + 2];
		if (minz > model->vertices[3 * i + 2])
			minz = model->vertices[3 * i + 2];
	}

	/* get eight vetexes*/

	model->bbox.vetexes[0][0] = minx;
	model->bbox.vetexes[0][1] = miny;
	model->bbox.vetexes[0][2] = minz;

	model->bbox.vetexes[1][0] = minx;
	model->bbox.vetexes[1][1] = maxy;
	model->bbox.vetexes[1][2] = minz;

	model->bbox.vetexes[2][0] = minx;
	model->bbox.vetexes[2][1] = maxy;
	model->bbox.vetexes[2][2] = maxz;

	model->bbox.vetexes[3][0] = minx;
	model->bbox.vetexes[3][1] = miny;
	model->bbox.vetexes[3][2] = maxz;

	model->bbox.vetexes[4][0] = maxx;
	model->bbox.vetexes[4][1] = miny;
	model->bbox.vetexes[4][2] = maxz;

	model->bbox.vetexes[5][0] = maxx;
	model->bbox.vetexes[5][1] = miny;
	model->bbox.vetexes[5][2] = minz;

	model->bbox.vetexes[6][0] = maxx;
	model->bbox.vetexes[6][1] = maxy;
	model->bbox.vetexes[6][2] = minz;

	model->bbox.vetexes[7][0] = maxx;
	model->bbox.vetexes[7][1] = maxy;
	model->bbox.vetexes[7][2] = maxz;


	model->bbox.bottom[0][0] = minx;
	model->bbox.bottom[0][1] = miny;
	model->bbox.bottom[0][2] = minz;

	model->bbox.bottom[1][0] = minx;
	model->bbox.bottom[1][1] = miny;
	model->bbox.bottom[1][2] = (minz+maxz)/2;

	model->bbox.bottom[2][0] = minx;
	model->bbox.bottom[2][1] = miny;
	model->bbox.bottom[2][2] = maxz;

	model->bbox.bottom[3][0] = (minx+maxx)/2;
	model->bbox.bottom[3][1] = miny;
	model->bbox.bottom[3][2] = maxz;

	model->bbox.bottom[4][0] = maxx;
	model->bbox.bottom[4][1] = miny;
	model->bbox.bottom[4][2] = maxz;

	model->bbox.bottom[5][0] = maxx;
	model->bbox.bottom[5][1] = miny;
	model->bbox.bottom[5][2] = (minz + maxz) / 2;

	model->bbox.bottom[6][0] = maxx;
	model->bbox.bottom[6][1] = miny;
	model->bbox.bottom[6][2] = minz;

	model->bbox.bottom[7][0] = (minx+maxx)/2;
	model->bbox.bottom[7][1] = miny;
	model->bbox.bottom[7][2] = minz;


	model->bbox.center[0] = model->position[0];
	model->bbox.center[1] = (miny+maxy)/2.0;
	model->bbox.center[2] = model->position[1];
	/*model->bbox.min[0] = minx;
	model->bbox.min[1] = miny;
	model->bbox.min[2] = minz; 

	model->bbox.max[0] = maxx;
	model->bbox.max[1] = maxy;
	model->bbox.max[2] = maxz;*/
	
}

/*glmCalBBox: Calculates the AABB of a model
*
* model - initialized GLMmodel structure
*/
GLvoid glmSetPosition(GLMmodel * model, GLMPosi posi)
{
	model->position[0] = posi.x;
	model->position[1] = posi.y; 
	model->position[2] = posi.z;
}

GLvoid glmSetLabel(GLMmodel * model, int tag)
{
	model->label = tag;
}

/* glmDraw: Renders the model to the current OpenGL context using the
* mode specified.
*
* model - initialized GLMmodel structure
* mode  - a bitwise OR of values describing what is to be rendered.
*             GLM_NONE     -  render with only vertices
*             GLM_FLAT     -  render with facet normals
*             GLM_SMOOTH   -  render with vertex normals
*             GLM_TEXTURE  -  render with texture coords
*             GLM_COLOR    -  render with colors (color material)
*             GLM_MATERIAL -  render with materials
*             GLM_COLOR and GLM_MATERIAL should not both be specified.
*             GLM_FLAT and GLM_SMOOTH should not both be specified.
*/
GLvoid glmDraw(GLMmodel * model, GLuint mode)
{
	static GLuint i;
	static GLMgroup *group;
	static GLMtriangle *triangle;
	static GLMmaterial *material;

	assert(model);
	assert(model->vertices);

	cout << model->seed_current <<"yes" << endl;
	glTranslatef(model->seeds[model->seed_current * 3 + 0],
		model->seeds[model->seed_current * 3 + 1],
		model->seeds[model->seed_current * 3 + 2]);

	glColor3f(0.0f, 0.0f, 0.8f);
	glutSolidSphere(0.15, 16, 16);

	glTranslatef(-model->seeds[model->seed_current * 3 + 0],
		-model->seeds[model->seed_current * 3 + 1],
		-model->seeds[model->seed_current * 3 + 2]);

	for (int i = 0; i < model->numtriangles; i++)
	{
		glBegin(GL_TRIANGLES);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
		//cout << "color size is" << _msize(model->iscolored)/sizeof(bool);
		
		glColor3f(0.71f, 0.71f, 0.71f);
		

		int ifindex = model->triangles[i].findex;
		glNormal3f(model->facetnorms[3 * ifindex], model->facetnorms[3 * ifindex + 1], model->facetnorms[3 * ifindex + 2]);

		glVertex3f(model->vertices[model->triangles[i].vindices[0] * 3 + 0],
			model->vertices[model->triangles[i].vindices[0] * 3 + 1],
			model->vertices[model->triangles[i].vindices[0] * 3 + 2]);

		glVertex3f(model->vertices[model->triangles[i].vindices[1] * 3 + 0],
			model->vertices[model->triangles[i].vindices[1] * 3 + 1],
			model->vertices[model->triangles[i].vindices[1] * 3 + 2]);

		glVertex3f(model->vertices[model->triangles[i].vindices[2] * 3 + 0],
			model->vertices[model->triangles[i].vindices[2] * 3 + 1],
			model->vertices[model->triangles[i].vindices[2] * 3 + 2]);

		glEnd();
	
	}
	

	

	//printf("%f %f %f", model->bbox.center[0], model->bbox.center[1], model->bbox.center[2]);
	/*draw the AABB lines*/
	/*
	glLineWidth(3);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm_red);
	glBegin(GL_LINES);

	glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);

	glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);
	glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);

	glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);
	glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);

	glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);
	glVertex3f(model->bbox.vetexes[4][0], model->bbox.vetexes[4][1], model->bbox.vetexes[4][2]);

	glVertex3f(model->bbox.vetexes[4][0], model->bbox.vetexes[4][1], model->bbox.vetexes[4][2]);
	glVertex3f(model->bbox.vetexes[5][0], model->bbox.vetexes[5][1], model->bbox.vetexes[5][2]);

	glVertex3f(model->bbox.vetexes[5][0], model->bbox.vetexes[5][1], model->bbox.vetexes[5][2]);
	glVertex3f(model->bbox.vetexes[6][0], model->bbox.vetexes[6][1], model->bbox.vetexes[6][2]);

	glVertex3f(model->bbox.vetexes[6][0], model->bbox.vetexes[6][1], model->bbox.vetexes[6][2]);
	glVertex3f(model->bbox.vetexes[7][0], model->bbox.vetexes[7][1], model->bbox.vetexes[7][2]);

	glVertex3f(model->bbox.vetexes[7][0], model->bbox.vetexes[7][1], model->bbox.vetexes[7][2]);
	glVertex3f(model->bbox.vetexes[4][0], model->bbox.vetexes[4][1], model->bbox.vetexes[4][2]);

	glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);

	glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	glVertex3f(model->bbox.vetexes[5][0], model->bbox.vetexes[5][1], model->bbox.vetexes[5][2]);

	glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);
	glVertex3f(model->bbox.vetexes[7][0], model->bbox.vetexes[7][1], model->bbox.vetexes[7][2]);
	
	glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);
	glVertex3f(model->bbox.vetexes[6][0], model->bbox.vetexes[6][1], model->bbox.vetexes[6][2]);

	glEnd();*/

	/*draw the AABB faces*/
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	////glColor4f(1, 1, 1, 1);
	//glColor3f(1.0, 1.0, 1.0);
	//glBegin(GL_POLYGON);
	//
	//glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	//glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);
	//glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);
	//glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);
	//

	//glEnd();

	
	//glTranslatef(model->position[0], model->position[1], model->position[2]);
	
}

/* glmList: Generates and returns a display list for the model using
* the mode specified.
*
* model - initialized GLMmodel structure
* mode  - a bitwise OR of values describing what is to be rendered.
*             GLM_NONE     -  render with only vertices
*             GLM_FLAT     -  render with facet normals
*             GLM_SMOOTH   -  render with vertex normals
*             GLM_TEXTURE  -  render with texture coords
*             GLM_COLOR    -  render with colors (color material)
*             GLM_MATERIAL -  render with materials
*             GLM_COLOR and GLM_MATERIAL should not both be specified.
* GLM_FLAT and GLM_SMOOTH should not both be specified.
*/
GLuint glmList(GLMmodel * model, GLuint mode)
{
	GLuint list;

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	glmDraw(model, mode);
	glEndList();

	return list;
}



GLuint glmListBBox(GLMmodel * model, GLuint mode)
{
	GLuint list;

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	glLineWidth(1);

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm_green);
	glBegin(GL_LINES);

	glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);

	glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);
	glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);

	glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);
	glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);

	glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);
	glVertex3f(model->bbox.vetexes[4][0], model->bbox.vetexes[4][1], model->bbox.vetexes[4][2]);

	glVertex3f(model->bbox.vetexes[4][0], model->bbox.vetexes[4][1], model->bbox.vetexes[4][2]);
	glVertex3f(model->bbox.vetexes[5][0], model->bbox.vetexes[5][1], model->bbox.vetexes[5][2]);

	glVertex3f(model->bbox.vetexes[5][0], model->bbox.vetexes[5][1], model->bbox.vetexes[5][2]);
	glVertex3f(model->bbox.vetexes[6][0], model->bbox.vetexes[6][1], model->bbox.vetexes[6][2]);

	glVertex3f(model->bbox.vetexes[6][0], model->bbox.vetexes[6][1], model->bbox.vetexes[6][2]);
	glVertex3f(model->bbox.vetexes[7][0], model->bbox.vetexes[7][1], model->bbox.vetexes[7][2]);

	glVertex3f(model->bbox.vetexes[7][0], model->bbox.vetexes[7][1], model->bbox.vetexes[7][2]);
	glVertex3f(model->bbox.vetexes[4][0], model->bbox.vetexes[4][1], model->bbox.vetexes[4][2]);

	glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	glVertex3f(model->bbox.vetexes[3][0], model->bbox.vetexes[3][1], model->bbox.vetexes[3][2]);

	glVertex3f(model->bbox.vetexes[0][0], model->bbox.vetexes[0][1], model->bbox.vetexes[0][2]);
	glVertex3f(model->bbox.vetexes[5][0], model->bbox.vetexes[5][1], model->bbox.vetexes[5][2]);

	glVertex3f(model->bbox.vetexes[2][0], model->bbox.vetexes[2][1], model->bbox.vetexes[2][2]);
	glVertex3f(model->bbox.vetexes[7][0], model->bbox.vetexes[7][1], model->bbox.vetexes[7][2]);

	glVertex3f(model->bbox.vetexes[1][0], model->bbox.vetexes[1][1], model->bbox.vetexes[1][2]);
	glVertex3f(model->bbox.vetexes[6][0], model->bbox.vetexes[6][1], model->bbox.vetexes[6][2]);

	glEnd();
	glEndList();

	return list;
}