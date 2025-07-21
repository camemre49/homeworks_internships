#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include "helper_functions.h"
#include "Rasterization.h"

using namespace tinyxml2;
using namespace std;

// Static member definitions
std::vector<std::vector<Color>> Scene::image;
std::vector<std::vector<double>> Scene::zBuffer;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (Scene::image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> depths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				depths.push_back(BACKGROUND_DEPTH);
			}

			Scene::image.push_back(rowOfColors);
			Scene::zBuffer.push_back(depths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				Scene::image[i][j].r = this->backgroundColor.r;
				Scene::image[i][j].g = this->backgroundColor.g;
				Scene::image[i][j].b = this->backgroundColor.b;
				Scene::zBuffer[i][j] = BACKGROUND_DEPTH;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}


/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(Scene::image[i][j].r) << " "
				 << makeBetweenZeroAnd255(Scene::image[i][j].g) << " "
				 << makeBetweenZeroAnd255(Scene::image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "./magick " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}

void Scene::forwardRenderingPipeline(Camera *camera)
{

	//this->backgroundColor = Color(255, 255, 255);
	Matrix4 viewingTransformationMatrix = camera->calculateViewTransformationMatrix();

	for (const auto &currentMesh : meshes)
	{
		for (const auto &currentTriangle : currentMesh->triangles) {

			
			Matrix4 modelTransformationMatrix = modellingTransformations(currentMesh);

			Matrix4 cameraTransformationMatrix = camera->calculateCameraTransformationMatrix();

			Matrix4 temporaryMatrix= multiplyMatrixWithMatrix(cameraTransformationMatrix, modelTransformationMatrix);


			Matrix4 projectionMatrix;

			if (camera->projectionType == 0)
			{
											//hata bunda gibi duruyor
				projectionMatrix = camera->calculateOrthographicViewTransformationMatrix();
			}
			else
			{
				Matrix4 pers2ortho = camera->perspective2Orthographic();
				//temporaryMatrix = multiplyMatrixWithMatrix(pers2ortho, temporaryMatrix);
				projectionMatrix = camera->calculatePerspectiveViewTransformationMatrix();
				//projectionMatrix = camera->calculateOrthographicViewTransformationMatrix();
			}

			Matrix4 wholeTransformationsMatrix = multiplyMatrixWithMatrix(projectionMatrix, temporaryMatrix);


			Vec4 verticesArray[3];

			for (int i = 0; i < 3; i++)
			{

				Vec4 vertex = Vec4(vertices[currentTriangle.vertexIds[i] - 1]->x,
									vertices[currentTriangle.vertexIds[i] - 1]->y,
									vertices[currentTriangle.vertexIds[i] - 1]->z,
									1,
									*(colorsOfVertices[vertices[currentTriangle.vertexIds[i] - 1]->colorId - 1]));
				
				Vec4 transformedVertex = multiplyMatrixWithVec4(wholeTransformationsMatrix, vertex);
				transformedVertex.color = vertex.color;
				transformedVertex = camera->projectionType ? perspectiveDivision(transformedVertex) : transformedVertex;
				verticesArray[i] = transformedVertex;
			}

			Vec4 pos={0,0,0,1};
			Vec4 transformedPos= multiplyMatrixWithVec4(projectionMatrix,pos);
			if (this->cullingEnabled && isBackFace(verticesArray,transformedPos))
				continue;

			//for (int i = 0; i < 3; i++)
			//{
			//	verticesArray[i] = multiplyMatrixWithVec4(projectionMatrix, verticesArray[i]);
			//	if(camera->projectionType)
			//	{
			//		verticesArray[i] = perspectiveDivision(verticesArray[i]);
			//	}
			//}

			if (currentMesh->type)
			{
				vector<Vec3> viewPortedVertices;
				for (auto & i : verticesArray)
				{
					Vec4 transformedVector = multiplyMatrixWithVec4(viewingTransformationMatrix, i);
					viewPortedVertices.emplace_back(transformedVector.x, transformedVector.y, transformedVector.z, -1, i.color);

				}
				rasterizeTriangle(
					TriangleStruct{viewPortedVertices[0], viewPortedVertices[1], viewPortedVertices[2]},
					camera->horRes,
					camera->verRes);
			}
			// Wireframe
			else
			{
				int indexes[3][2]={{0,1},{1,2},{2,0}};
				for (auto i: indexes)
				{
					//std::cout <<"i[2]"<<i[2] <<"i[0]: " << i[0] << " i[1]: " << i[1] << std::endl;
					Vec4 point1 = verticesArray[i[0]];
					Vec4 point2 = verticesArray[i[1]];
					bool line_visibility  = apply_liang_barsky(point1, point2);

					if (line_visibility)
					{
						Vec4 p1 = point1;
						Vec4 p2 = point2;

						Vec4 p1_view = multiplyMatrixWithVec4(viewingTransformationMatrix, p1);
						Vec4 p2_view = multiplyMatrixWithVec4(viewingTransformationMatrix, p2);

						Vec3 viewportTransformedP1 = Vec3(p1_view.x, p1_view.y, p1_view.z, p1.color);
						Vec3 viewportTransformedP2 = Vec3(p2_view.x, p2_view.y, p2_view.z, p2.color);

						LineStruct lines = LineStruct{viewportTransformedP1, viewportTransformedP2};
						rasterizeLine(lines);

						//std::pair<Vec3, Vec3> vertices = std::make_pair(viewportTransformedP1, viewportTransformedP2);
						//lineRasterization1(vertices);


					}
				}
			}
		}
	}
}
//enum types
//{
//	ROTATION_ = Scene::rotations,
//	TRANSLATION_ = Scene::translations,
//	SCALING_ = Scene::scalings
//};

/**
 * Apply modelling transformations
 */

//std::vector<std::vector*>

Matrix4 Scene::modellingTransformations(Mesh *mesh)
{
	Matrix4 modellingMatrix = getIdentityMatrix();
	for (int i = 0; i < (mesh->numberOfTransformations); i++)
	{
		if (mesh->transformationTypes[i] == 'r')
		{
			modellingMatrix = multiplyMatrixWithMatrix(
				Matrix4(rotations[mesh->transformationIds[i] - 1]->getTransformationMatrix()),
				modellingMatrix
				);
		}

		else if (mesh->transformationTypes[i] == 't')
		{
			modellingMatrix = multiplyMatrixWithMatrix(
				Matrix4(translations[mesh->transformationIds[i] - 1]->getTransformationMatrix()),
				modellingMatrix);
		}

		else if (mesh->transformationTypes[i] == 's')
		{
			modellingMatrix = multiplyMatrixWithMatrix(
				Matrix4(scalings[mesh->transformationIds[i] - 1]->getTransformationMatrix()),
				modellingMatrix);
		}
	}
	return modellingMatrix;
}

bool Scene::isBackFace(Vec4 vertices[3],Vec4 cameraTranformed)
{
	// Calculate the vectors of the triangle edges
	Vec3 vector1 = subtractVec3(Vec3(vertices[1].x, vertices[1].y, vertices[1].z), Vec3(vertices[0].x, vertices[0].y, vertices[0].z));
	Vec3 vector2 = subtractVec3(Vec3(vertices[2].x, vertices[2].y, vertices[2].z), Vec3(vertices[0].x, vertices[0].y, vertices[0].z));

	// Compute the normal to the triangle's surface
	Vec3 normal = normalizeVec3(crossProductVec3(vector1, vector2));

	// Check if the triangle is facing away from the camera (culled)
	return dotProductVec3(normal, Vec3(vertices[1].x-cameraTranformed.x, vertices[1].y-cameraTranformed.y, vertices[1].z-cameraTranformed.z)) < 0;
	//return dotProductVec3(normal, Vec3(vertices[0].x, vertices[0].y, vertices[0].z)) < 0;
}


