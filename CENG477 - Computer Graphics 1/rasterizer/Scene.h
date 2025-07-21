#ifndef _SCENE_H_
#define _SCENE_H_
#include "Vec3.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;

	std::vector<Camera *> cameras;
	std::vector<Vec3 *> vertices;
	std::vector<Color *> colorsOfVertices;
	std::vector<Scaling *> scalings;
	std::vector<Rotation *> rotations;
	std::vector<Translation *> translations;
	std::vector<Mesh *> meshes;

	// store scalings, rotations, translations in a single vector, use their own pointers
	

	static std::vector<std::vector<Color> > image;
	static std::vector<std::vector<double>> zBuffer;


	/**
	 * Constants of the scene
	 */
	double const BACKGROUND_DEPTH = 1.0001; // Depth of the background in the scene. It is a little greater than 1 which is the back of the transformed scene

	// Constants of the scene


	Scene(const char *xmlPath);

	void initializeImage(Camera *camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera *camera);
	void convertPPMToPNG(std::string ppmFileName, int osType);
	void forwardRenderingPipeline(Camera *camera);

	Matrix4 modellingTransformations(Mesh *mesh);
	bool isBackFace(Vec4 vertices[3], Vec4 cameraTranformed);
};

#endif
