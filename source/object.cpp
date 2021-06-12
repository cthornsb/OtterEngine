#include <iostream>
#include <algorithm> // find
#include <float.h>

#include "Globals.hpp"
#include "object.hpp"
#include "camera.hpp"
#include "Vector.hpp"
#include "OTTTexture.hpp"
#include "OTTShader.hpp"
#include "OTTWindow3D.hpp"

object::object() :
	built(false),
	bDrawOrigin(false),
	bDrawNormals(false),
	bHidden(true), // Objects hidden until a shader is set
	reservedVertices(0),
	reservedPolygons(0),
	position(),
	position0(),
	rotation(identityMatrix3),
	uX(1.f, 0.f, 0.f),
	uY(0.f, 1.f, 0.f),
	uZ(0.f, 0.f, 1.f),
	center(),
	scaleFactor(1.f, 1.f, 1.f),
	pitchAngle(0, 0, 2 * pi),
	rollAngle(0, 0, 2 * pi),
	yawAngle(0, 0, 2 * pi),
	maxSize(),
	minSize(),
	vertices(),
	polys(),
	children(),
	parent(0x0),
	shader(0x0),
	texture(0x0),
	ambientColor(1.f, 1.f, 1.f),
	modelMatrix()
{
	for (int i = 0; i < 3; i++) {
		maxSize[i] = -FLT_MAX;
		minSize[i] = +FLT_MAX;
	}
}

/** Object position constructor
  */
object::object(const Vector3 & pos_) :
	object()
{
	position = pos_;
	position0 = pos_;
}

Matrix4* object::getModelMatrix() {
	// Update the view matrix as the camera may have moved
	modelMatrix.setSubMatrixColumn(0, uX * scaleFactor[0], position[0]);
	modelMatrix.setSubMatrixColumn(1, uY * scaleFactor[1], position[1]);
	modelMatrix.setSubMatrixColumn(2, uZ * scaleFactor[2], position[2]);
	modelMatrix.setSubMatrixColumn(3, zeroVector, 1.f);
	return &modelMatrix;
}

Matrix4* object::getModelMatrix(const Matrix4& mat) {
	// Update the view matrix as the camera may have moved
	modelMatrix.setSubMatrixColumn(0, uX * scaleFactor[0], position[0]);
	modelMatrix.setSubMatrixColumn(1, uY * scaleFactor[1], position[1]);
	modelMatrix.setSubMatrixColumn(2, uZ * scaleFactor[2], position[2]);
	modelMatrix.setSubMatrixColumn(3, zeroVector, 1.f);
	modelMatrix *= mat;
	return &modelMatrix;
}

void object::rotate(const float& theta, const float& phi, const float& psi){
	// Update the rotation matrix
	rotation.setRotation((pitchAngle += theta), (rollAngle += psi), (yawAngle += phi));

	// Update rotation of child objects
	updateRotation();
}

void object::move(const Vector3 &offset){
	position += offset;
}

void object::setRotation(const float& theta, const float& phi, const float& psi){
	// Update the rotation matrix
	rotation.setRotation((pitchAngle = theta), (rollAngle = psi), (yawAngle = phi));

	// Update rotation of child objects
	updateRotation();
}

void object::setPosition(const Vector3 &pos){
	position = pos;
}

void object::setShader(const OTTShader3D* shdr, bool bSetChildren/*=false*/) {
	bHidden = (shdr == 0x0);
	shader = shdr; 
	if (bSetChildren) {
		for (std::vector<object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
			(*ch)->setShader(shdr);
		}
	}
}

void object::setTexture(OTTTexture* txt, bool bSetChildren/*=false*/) {
	texture = txt;
	if (bSetChildren) {
		for (std::vector<object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
			(*ch)->setTexture(txt);
		}
	}
}

void object::resetPosition(){
	position = position0;
}

void object::renderAllVertices(camera* cam) {
	for (std::vector<Vertex>::iterator vert = vertices.begin(); vert != vertices.end(); vert++) {
		cam->render(*vert);
	}
}

void object::addChild(object* child) {
	children.push_back(child);
	if (!child->setParent(this)) {
		std::cout << " Object: [warning] Child object already had a parent, this may result in undefined behavior!" << std::endl;
	}
}

void object::removeChild(object* child) {
	auto ch = std::find(children.begin(), children.end(), child);
	if (ch != children.end()) { // Remove the child and unlink it from this object
		(*ch)->parent = 0x0;
		children.erase(ch);
	}
	else {
		std::cout << " Object: [warning] Child object not found. Cannot remove" << std::endl;
	}
}

void object::build() {
	if (built)
		return;

	// Finalize the object geometry
	this->userBuild();

	// Transfer geometry to the gpu
	polys.finalize();

	// Free geometry memory
	vertices.free();
	if(!bDrawNormals)
		polys.free();

	// Build any child objects
	for (std::vector<object*>::const_iterator ch = children.cbegin(); ch != children.cend(); ch++) {
		(*ch)->build();
	}

	built = true;
}

void object::draw(OTTWindow3D* win, Matrix4* proj, Matrix4* view) {
	if (bHidden) // Object is hidden
		return;

	// Set object's ambient color (used by some shaders)
	win->setDrawColor(ambientColor);

	// Setup MVP matrices
	Matrix4* model = getModelMatrix();
	Matrix4 mvp = Matrix4::concatenate(proj, view, model);

	// Draw the parent
	if (polys.getVertexVBO() && (shader && !shader->isHidden())) { // Object must have a shader to be rendered		
		// Enable shader and set uniform transformation matrices (used by most shaders)
		shader->enableShader();
		shader->setMatrix4("MVP", mvp);
		shader->setMatrix4("model", model);
		shader->setMatrix4("view", view);
		shader->setMatrix4("proj", proj);

		// Draw the parent geometry
		shader->enableObjectShader(this);
		win->drawObject(this);
		shader->disableObjectShader(this);

		// Disable shader program
		shader->disableShader();
	}

	// Draw all child objects (if any)
	for (std::vector<object*>::const_iterator ch = children.cbegin(); ch != children.cend(); ch++) { 
		const OTTShader3D* childShader = (*ch)->getShader(); // Child objects may have different shaders than the parent
		if (!childShader)
			continue;

		// Update child model matrix
		Matrix4* submodel = (*ch)->getModelMatrix(*model);
		Matrix4 submvp = Matrix4::concatenate(proj, view, submodel);

		// Set uniform transformation matrices (used by most shaders)
		childShader->enableShader();
		childShader->setMatrix4("MVP", submvp);
		childShader->setMatrix4("model", submodel);
		childShader->setMatrix4("view", view);
		childShader->setMatrix4("proj", proj);
		
		childShader->enableObjectShader(*ch);
		win->drawObject(*ch);
		childShader->disableObjectShader(*ch);
		childShader->disableShader();
	}

	// Debugging
	if (bDrawOrigin || bDrawNormals) {
		OTTShader* defShader = win->getShader(ShaderType::DEFAULT);
		defShader->enableShader();
		defShader->setMatrix4("MVP", mvp);
		if (bDrawOrigin) { // Draw object unit vectors
			win->drawAxes();
		}
		if (bDrawNormals) { // Draw face normals
			win->setDrawColor(Colors::CYAN);
			for (auto tri = polys.getPolygons()->cbegin(); tri != polys.getPolygons()->cend(); tri++) {
				Vector3 base = tri->getInitialCenterPoint();
				Vector3 tip = base + tri->getInitialNormal() * 0.25f;
				win->drawLine(base, tip);
			}
		}
		defShader->disableShader();
	}
}

void object::setSizeX(const float& min_, const float& max_) {
	minSize[0] = min_;
	maxSize[0] = max_;
}

void object::setSizeY(const float& min_, const float& max_) {
	minSize[1] = min_;
	maxSize[1] = max_;
}

void object::setSizeZ(const float& min_, const float& max_) {
	minSize[2] = min_;
	maxSize[2] = max_;
}

bool object::setParent(const object* obj) { 
	bool retval = (parent == 0x0);
	parent = obj;
	return retval;
}

void object::updateRotation() {
	uX = rotation.transform(unitVectorX);
	uY = rotation.transform(unitVectorY);
	uZ = rotation.transform(unitVectorZ);
}

void object::reserve(const size_t& nVert, const size_t& nPoly/*=0*/) {
	reserveVertices(nVert);
	if (nPoly == 0) {
		reservePolygons(nVert);
	}
	else {
		reservePolygons(nPoly);
	}
}

void object::reserveVertices(const size_t& nVert) {
	reservedVertices = nVert;
	vertices.reserve(nVert);
}

void object::reservePolygons(const size_t& nPoly) {
	reservedPolygons = nPoly;
	polys.reserve(nPoly);
}

void object::transform(const Matrix3* mat){
	// Transform all object vertices
	for(std::vector<Vertex>::iterator vert = vertices.begin(); vert != vertices.end(); vert++)
		vert->transform(mat);
	
	// Update the normals of all polygons
	for(std::vector<triangle>::iterator tri = polys.begin(); tri != polys.end(); tri++)
		tri->update();
}

Vertex* object::addVertex(const float&x, const float&y, const float&z, const float& u, const float& v){
	return addVertex(Vector3(x, y, z), Vector2(u, v));
}

Vertex* object::addVertex(const Vector3& vec, const Vector2& uv) {
	if (vertices.size() > reservedVertices){
		std::cout << " Object: [warning] Not enough memory reserved for vertex vector, this may result in undefined behavior!" << std::endl;
	}
	vertices.add(vec, this);
	for (int i = 0; i < 3; i++) { // Update the size of the bounding box
		if (vec[i] > maxSize[i])
			maxSize[i] = vec[i];
		if (vec[i] < minSize[i])
			minSize[i] = vec[i];
	}
	vertices.back()->setTextureCoordinates(uv);
	return vertices.back();
}

void object::addTriangle(Vertex* v0, Vertex* v1, Vertex* v2) {
	if (polys.size() > reservedPolygons) {
		std::cout << " Object: [warning] Not enough memory reserved for polygon vector, this may result in undefined behavior!" << std::endl;
	}
	polys.add(v0, v1, v2, this);
}

void object::addTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2){
	addTriangle(vertices[i0], vertices[i1], vertices[i2]);
}

void object::addTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2,
	const Vector2& uv0, const Vector2& uv1, const Vector2& uv2)
{
	addTriangle(vertices[i0], vertices[i1], vertices[i2]);
	polys.modifyTextureMap(uv0, uv1, uv2);
}

void object::addQuad(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3) {
	addTriangle(v0, v1, v2);
	addTriangle(v2, v3, v0);
}

void object::addQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3) {
	addTriangle(vertices[i0], vertices[i1], vertices[i2]);
	addTriangle(vertices[i2], vertices[i3], vertices[i0]);
}

void object::addQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3,
	const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3)
{
	addTriangle(vertices[i0], vertices[i1], vertices[i2]);
	polys.modifyTextureMap(uv0, uv1, uv2);
	addTriangle(vertices[i2], vertices[i3], vertices[i0]);
	polys.modifyTextureMap(uv2, uv3, uv0);
}

void object::addStaticTriangle(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2) {
	polys.add(i0, i1, i2, &vertices, this);
}

void object::addStaticQuad(const unsigned short& i0, const unsigned short& i1, const unsigned short& i2, const unsigned short& i3) {
	// Eventually this will use a quad face, but for now we get two triangles
	polys.add(i0, i1, i2, &vertices, this);
	polys.add(i2, i3, i0, &vertices, this);
}

void SubObject::addSubGeometry(Vertex* v0, Vertex* v1, Vertex* v2) {
	addTriangle(v0, v1, v2);
}

void SubObject::addSubGeometry(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3) {
	addTriangle(v0, v1, v2);
	addTriangle(v2, v3, v0);
}
