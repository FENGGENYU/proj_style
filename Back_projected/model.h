#ifndef _MODEL_H
#define _MODEL_H


#include <gl\glut.h>
//#include "CommonZ.h"
#include <string>

#ifndef M_PI
#define M_PI 3.14159265f
#endif
#define COLOR "blue"

#define GLM_NONE                    (0)					/* render with only vertices */
#define GLM_FLAT                      (1 << 0)			/* render with facet normals */
#define GLM_SMOOTH				   (1 << 1)			/* render with vertex normals */
#define GLM_TEXTURE				   (1 << 2)			/* render with texture coords */
#define GLM_COLOR                   (1 << 3)			/* render with colors */
#define GLM_MATERIAL              (1 << 4)			/* render with materials */

const GLfloat glm_red[4] = {1.0,0.0,0.0,0.0};
const GLfloat glm_green[4] = { 0.0, 1.0, 0.0, 0.0 };
const GLfloat glm_blue[4] = { 0.0, 0.0, 1.0, 0.0 }; 
const GLfloat glm_yellow[4] = { 1.0, 1.0, 0.0, 0.0 }; 
const GLfloat glm_purple[4] = { 1.0, 0.0, 1.0, 0.0 };

typedef struct _GLMPosi
{
	GLfloat x;
	GLfloat y;
	GLfloat z;

}GLMPosi;

/* GLMmaterial: Structure that defines a material in a model.
*/
typedef struct _GLMmaterial {
	char *name;			        /* name of material */
	GLfloat diffuse[4];		    /* diffuse component */
	GLfloat ambient[4];		/* ambient component */
	GLfloat specular[4];	    /* specular component */
	GLfloat emmissive[4];	/* emmissive component */
	GLfloat shininess;		    /* specular exponent */
} GLMmaterial;

/* GLMtriangle: Structure that defines a triangle in a model.
*/
typedef struct _GLMtriangle {
	GLuint vindices[3];		/* array of triangle vertex indices */
	GLuint nindices[3];		/* array of triangle normal indices */
	GLuint tindices[3];		    /* array of triangle texture cord indices */
	GLuint findex;		        /* index of triangle facet normal */
} GLMtriangle;

/* GLMgroup: Structure that defines a group in a model.
*/
typedef struct _GLMgroup {
	char *name;						 /* name of this group */
	GLuint numtriangles;		 /* number of triangles in this group */
	GLuint *triangles;				 /* array of triangle indices */
	GLuint material;				 /* index to material for group */
	struct _GLMgroup *next;	 /* pointer to next group in model */
} GLMgroup;

typedef struct _GLMAABB{
	GLfloat vetexes[8][3];
	GLfloat center[3];
	GLfloat bottom[8][3];	 /* used to calculate the distance between models*/
}GLAABB;

/* GLMmodel: Structure that defines a model.
*/
typedef struct _GLMmodel {
	char *pathname;		/* path to this model */
	char *mtllibname;		/* name of the material library */
	int label;		            /* label of the model*/

	//add else
	bool *iscolored;

	bool *isstyle;

	GLuint numseeds;
	GLuint seed_current;
	GLfloat *seeds;

	//v
	GLuint numvertices;		/* number of vertices in model */
	GLfloat *vertices;		    /* array of vertices  */

	//vn
	GLuint numnormals;		/* number of normals in model */
	GLfloat *normals;		    /* array of normals */
	//vt
	GLuint numtexcoords;	/* number of texcoords in model */
	GLfloat *texcoords;		/* array of texture coordinates */

	GLuint numfacetnorms;	/* number of facetnorms in model */
	GLfloat *facetnorms;	        /* array of facetnorms */
	//f
	GLuint numtriangles;	/* number of triangles in model */
	GLMtriangle *triangles;	/* array of triangles */

	GLuint nummaterials;	    /* number of materials in model */
	GLMmaterial *materials;	/* array of materials */

	GLuint numgroups;		/* number of groups in model */
	GLMgroup *groups;		/* linked list of groups */

	GLAABB bbox;            /* AABB of the model*/
	GLfloat position[3];	 /* position of the model */
	

} GLMmodel;


/* glmUnitize: "unitize" a model by translating it to the origin and
* scaling it to fit in a unit cube around the origin.  Returns the
* scale factor used.
*
* model - properly initialized GLMmodel structure
*/
GLfloat glmUnitize(GLMmodel * model);

GLvoid glmRotate(GLMmodel * model);

/* glmCalBBox: Calculates the AABB of a model 
*
* model      - initialized GLMmodel structure
*/
GLvoid glmCalBBox(GLMmodel * model);

/* glmFacetNormals: Generates facet normals for a model (by taking the
* cross product of the two vectors derived from the sides of each
* triangle).  Assumes a counter-clockwise winding.
*
* model - initialized GLMmodel structure
*/
GLvoid glmFacetNormals(GLMmodel * model);

/* glmVertexNormals: Generates smooth vertex normals for a model.
* First builds a list of all the triangles each vertex is in.  Then
* loops through each vertex in the the list averaging all the facet
* normals of the triangles each vertex is in.  Finally, sets the
* normal index in the triangle for the vertex to the generated smooth
* normal.  If the dot product of a facet normal and the facet normal
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
GLvoid glmVertexNormals(GLMmodel * model, GLfloat angle);

/* glmDelete: Deletes a GLMmodel structure.
*
* model - initialized GLMmodel structure
*/
GLvoid glmDelete(GLMmodel * model);

/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
* Returns a pointer to the created object which should be free'd with
* glmDelete().
*
* filename - name of the file containing the Wavefront .OBJ format data.
*/
GLMmodel *glmReadOBJ(char *filename);

/* glmWriteOBJ: Writes a model description in Wavefront .OBJ format to
* a file.
*
* model    - initialized GLMmodel structure
* filename - name of the file to write the Wavefront .OBJ format data to
* mode     - a bitwise or of values describing what is written to the file
*            GLM_NONE    -  write only vertices
*            GLM_FLAT    -  write facet normals
*            GLM_SMOOTH  -  write vertex normals
*            GLM_TEXTURE -  write texture coords
*            GLM_FLAT and GLM_SMOOTH should not both be specified.
*/
GLvoid glmWriteOBJ(GLMmodel * model, char *filename, GLuint mode);


GLvoid glmSetLabel(GLMmodel * model, int tag);
GLvoid glmSetPosition(GLMmodel *model, GLMPosi posi);
GLvoid glmCalBBox(GLMmodel * model);
GLvoid glmTranslate(GLMmodel *model);


/* glmDraw: Renders the model to the current OpenGL context using the
* mode specified.
*
* model    - initialized GLMmodel structure
* mode     - a bitwise OR of values describing what is to be rendered.
*            GLM_NONE    -  render with only vertices
*            GLM_FLAT    -  render with facet normals
*            GLM_SMOOTH  -  render with vertex normals
*            GLM_TEXTURE -  render with texture coords
*            GLM_FLAT and GLM_SMOOTH should not both be specified.
*/
GLvoid glmDraw(GLMmodel * model, GLuint mode);

/* glmList: Generates and returns a display list for the model using
* the mode specified.
*
* model    - initialized GLMmodel structure
* mode     - a bitwise OR of values describing what is to be rendered.
*            GLM_NONE    -  render with only vertices
*            GLM_FLAT    -  render with facet normals
*            GLM_SMOOTH  -  render with vertex normals
*            GLM_TEXTURE -  render with texture coords
*            GLM_FLAT and GLM_SMOOTH should not both be specified.
*/
GLuint glmList(GLMmodel * model, GLuint mode);

GLuint glmListBBox(GLMmodel * model, GLuint mode);


#endif // !_MODEL_H

