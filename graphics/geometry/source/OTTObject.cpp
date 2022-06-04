
#include "OTTObject.hpp"

#include <graphics/3d/OTTCamera.hpp>
#include <graphics/3d/OTTWindow3d.hpp>
#include <math/OTTConstants.hpp>

#include <iostream>
#include <algorithm> // find
#include <float.h>

ott::Object::Object() :
	built(false),
	bDrawOrigin(false),
	bDrawNormals(false),
	bHidden(true), // Objects hidden until a shader is set
	reservedVertices(0),
	reservedPolygons(0),
	position(),
	position0(),
	rotation(constants::kIdentityMatrix3),
	uX(1.f, 0.f, 0.f),
	uY(0.f, 1.f, 0.f),
	uZ(0.f, 0.f, 1.f),
	center(),
	scaleFactor(1.f, 1.f, 1.f),
	pitchAngle(0, 0, 2 * constants::kPi),
	rollAngle(0, 0, 2 * constants::kPi),
	yawAngle(0, 0, 2 * constants::kPi),
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
	for (int32_t i = 0; i < 3; i++) {
		maxSize[i] = -FLT_MAX;
		minSize[i] = +FLT_MAX;
	}
}

/** Object position constructor
  */
ott::Object::Object(const Vector3 & pos_) :
	Object()
{
	position = pos_;
	position0 = pos_;
}

ott::Matrix4* ott::Object::ModelMatrix() {
	// Update the view matrix as the camera may have moved
	modelMatrix.SetSubMatrixColumn(0, uX * scaleFactor[0], position[0]);
	modelMatrix.SetSubMatrixColumn(1, uY * scaleFactor[1], position[1]);
	modelMatrix.SetSubMatrixColumn(2, uZ * scaleFactor[2], position[2]);
	modelMatrix.SetSubMatrixColumn(3, constants::kZeroVector, 1.f);
	return &modelMatrix;
}

ott::Matrix4* ott::Object::ModelMatrix(const Matrix4& mat) {
	// Update the view matrix as the camera may have moved
	modelMatrix.SetSubMatrixColumn(0, uX * scaleFactor[0], position[0]);
	modelMatrix.SetSubMatrixColumn(1, uY * scaleFactor[1], position[1]);
	modelMatrix.SetSubMatrixColumn(2, uZ * scaleFactor[2], position[2]);
	modelMatrix.SetSubMatrixColumn(3, constants::kZeroVector, 1.f);
	modelMatrix *= mat;
	return &modelMatrix;
}

void ott::Object::Rotate(const float& theta, const float& phi, const float& psi){
	// Update the rotation matrix
	rotation.SetRotation((pitchAngle += theta), (rollAngle += psi), (yawAngle += phi));

	// Update rotation of child objects
	this->UpdateRotation();
}

void ott::Object::Move(const Vector3 &offset){
	position += offset;
}

void ott::Object::SetRotation(const float& theta, const float& phi, const float& psi){
	// Update the rotation matrix
	rotation.SetRotation((pitchAngle = theta), (rollAngle = psi), (yawAngle = phi));

	// Update rotation of child objects
	this->UpdateRotation();
}

void ott::Object::SetPosition(const Vector3 &pos){
	position = pos;
}

void ott::Object::SetShader(const Shader3d* shdr, bool bSetChildren/*=false*/) {
	bHidden = (shdr == 0x0);
	shader = shdr; 
	if (bSetChildren) {
		for (std::vector<Object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
			(*ch)->SetShader(shdr);
		}
	}
}

void ott::Object::SetTexture(Texture* txt, bool bSetChildren/*=false*/) {
	texture = txt;
	if (bSetChildren) {
		for (std::vector<Object*>::iterator ch = children.begin(); ch != children.end(); ch++) {
			(*ch)->SetTexture(txt);
		}
	}
}

void ott::Object::ResetPosition(){
	position = position0;
}

void ott::Object::RenderAllVertices(Camera* cam) {
#if defined(SOFTWARE_RENDERER)
	for (std::vector<Vertex>::iterator vert = vertices.Begin(); vert != vertices.End(); vert++) {
		cam->Render(*vert);
	}
#endif // #if defined(SOFTWARE_RENDERER)
}

void ott::Object::AddChild(Object* child) {
	children.push_back(child);
	if (!child->SetParent(this)) {
		std::cout << " Object: [warning] Child object already had a parent, this may result in undefined behavior!" << std::endl;
	}
}

void ott::Object::RemoveChild(Object* child) {
	auto ch = std::find(children.begin(), children.end(), child);
	if (ch != children.end()) { // Remove the child and unlink it from this object
		(*ch)->parent = 0x0;
		children.erase(ch);
	}
	else {
		std::cout << " Object: [warning] Child object not found. Cannot remove" << std::endl;
	}
}

void ott::Object::Build() {
	if (built == true) {
		return;
	}

	// Finalize the object geometry
	this->OnUserBuild();

	// Transfer geometry to the gpu
	polys.Finalize();

	// Free geometry memory
	vertices.Free();
	if(!bDrawNormals)
		polys.Free();

	// Build any child objects
	for (std::vector<Object*>::const_iterator ch = children.cbegin(); ch != children.cend(); ch++) {
		(*ch)->Build();
	}

	built = true;
}

void ott::Object::Draw(Window3d* win, Matrix4* proj, Matrix4* view) {
	if (bHidden) // Object is hidden
		return;

	// Set object's ambient color (used by some shaders)
	win->SetDrawColor(ambientColor);

	// Setup MVP matrices
	Matrix4* model = this->ModelMatrix();
	Matrix4 mvp = Matrix4::Concatenate(proj, view, model);

	// Draw the parent
	if (polys.VertexVBO() && (shader && !shader->IsHidden())) { // Object must have a shader to be rendered		
		// Enable shader and set uniform transformation matrices (used by most shaders)
		shader->EnableShader();
		shader->SetMatrix4("MVP", mvp);
		shader->SetMatrix4("model", model);
		shader->SetMatrix4("view", view);
		shader->SetMatrix4("proj", proj);

		// Draw the parent geometry
		shader->EnableObjectShader(this);
		win->DrawObject(this);
		shader->DisableObjectShader(this);

		// Disable shader program
		shader->DisableShader();
	}

	// Draw all child objects (if any)
	for (std::vector<Object*>::const_iterator ch = children.cbegin(); ch != children.cend(); ch++) { 
		const Shader3d* childShader = (*ch)->GetShader(); // Child objects may have different shaders than the parent
		if (!childShader)
			continue;

		// Update child model matrix
		Matrix4* submodel = (*ch)->ModelMatrix(*model);
		Matrix4 submvp = Matrix4::Concatenate(proj, view, submodel);

		// Set uniform transformation matrices (used by most shaders)
		childShader->EnableShader();
		childShader->SetMatrix4("MVP", submvp);
		childShader->SetMatrix4("model", submodel);
		childShader->SetMatrix4("view", view);
		childShader->SetMatrix4("proj", proj);
		
		childShader->EnableObjectShader(*ch);
		win->DrawObject(*ch);
		childShader->DisableObjectShader(*ch);
		childShader->DisableShader();
	}

	// Debugging
	if (bDrawOrigin || bDrawNormals) {
		Shader* defShader = win->GetShader(ShaderType::Default);
		defShader->EnableShader();
		defShader->SetMatrix4("MVP", mvp);
		if (bDrawOrigin) { // Draw object unit vectors
			win->DrawAxes();
		}
		if (bDrawNormals) { // Draw face normals
			win->SetDrawColor(colors::Cyan);
			for (auto tri = polys.Polygons()->cbegin(); tri != polys.Polygons()->cend(); tri++) {
				Vector3 base = tri->InitialCenterPoint();
				Vector3 tip = base + tri->InitialNormal() * 0.25f;
				win->DrawLine(base, tip);
			}
		}
		defShader->DisableShader();
	}
}

void ott::Object::SetSizeX(const float& min_, const float& max_) {
	minSize[0] = min_;
	maxSize[0] = max_;
}

void ott::Object::SetSizeY(const float& min_, const float& max_) {
	minSize[1] = min_;
	maxSize[1] = max_;
}

void ott::Object::SetSizeZ(const float& min_, const float& max_) {
	minSize[2] = min_;
	maxSize[2] = max_;
}

bool ott::Object::SetParent(const Object* obj) { 
	bool retval = (parent == 0x0);
	parent = obj;
	return retval;
}

void ott::Object::UpdateRotation() {
	uX = rotation.Transform(constants::kUnitVectorX);
	uY = rotation.Transform(constants::kUnitVectorY);
	uZ = rotation.Transform(constants::kUnitVectorZ);
}

void ott::Object::Reserve(const size_t& nVert, const size_t& nPoly/*=0*/) {
	this->ReserveVertices(nVert);
	if (nPoly == 0) {
		this->ReservePolygons(nVert);
	}
	else {
		this->ReservePolygons(nPoly);
	}
}

void ott::Object::ReserveVertices(const size_t& nVert) {
	reservedVertices = nVert;
	vertices.Reserve(nVert);
}

void ott::Object::ReservePolygons(const size_t& nPoly) {
	reservedPolygons = nPoly;
	polys.Reserve(nPoly);
}

void ott::Object::Transform(const Matrix3* mat){
	// Transform all object vertices
	for (std::vector<Vertex>::iterator vert = vertices.Begin(); vert != vertices.End(); vert++) {
		vert->Transform(mat);
	}
	
	// Update the normals of all polygons
	for (std::vector<Polygon>::iterator tri = polys.Begin(); tri != polys.End(); tri++) {
		tri->Update();
	}
}

ott::Vertex* ott::Object::AddVertex(const float&x, const float&y, const float&z, const float& u, const float& v){
	return this->AddVertex(Vector3(x, y, z), Vector2(u, v));
}

ott::Vertex* ott::Object::AddVertex(const Vector3& vec, const Vector2& uv) {
	if (vertices.size() > reservedVertices){
		std::cout << " Object: [warning] Not enough memory reserved for vertex vector, this may result in undefined behavior!" << std::endl;
	}
	vertices.Add(vec, this);
	for (int32_t i = 0; i < 3; i++) { // Update the size of the bounding box
		if (vec[i] > maxSize[i])
			maxSize[i] = vec[i];
		if (vec[i] < minSize[i])
			minSize[i] = vec[i];
	}
	vertices.Back()->texCoords = uv;
	return vertices.Back();
}

void ott::Object::AddTriangle(const Vertex* v0, const Vertex* v1, const Vertex* v2) {
	if (polys.Size() > reservedPolygons) {
		std::cout << " Object: [warning] Not enough memory reserved for polygon vector, this may result in undefined behavior!" << std::endl;
	}
	polys.Add(v0, v1, v2, this);
}

void ott::Object::AddTriangle(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2){
	this->AddTriangle(vertices[i0], vertices[i1], vertices[i2]);
}

void ott::Object::AddTriangle(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2,
	const Vector2& uv0, const Vector2& uv1, const Vector2& uv2)
{
	this->AddTriangle(vertices[i0], vertices[i1], vertices[i2]);
	polys.ModifyTextureMap(uv0, uv1, uv2);
}

void ott::Object::AddQuad(const Vertex* v0, const Vertex* v1, const Vertex* v2, const Vertex* v3) {
	this->AddTriangle(v0, v1, v2);
	this->AddTriangle(v2, v3, v0);
}

void ott::Object::AddQuad(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, const uint16_t& i3) {
	this->AddTriangle(vertices[i0], vertices[i1], vertices[i2]);
	this->AddTriangle(vertices[i2], vertices[i3], vertices[i0]);
}

void ott::Object::AddQuad(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, const uint16_t& i3,
	const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3)
{
	this->AddTriangle(vertices[i0], vertices[i1], vertices[i2]);
	polys.ModifyTextureMap(uv0, uv1, uv2);
	this->AddTriangle(vertices[i2], vertices[i3], vertices[i0]);
	polys.ModifyTextureMap(uv2, uv3, uv0);
}

void ott::Object::AddStaticTriangle(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2) {
	polys.Add(i0, i1, i2, &vertices, this);
}

void ott::Object::AddStaticQuad(const uint16_t& i0, const uint16_t& i1, const uint16_t& i2, const uint16_t& i3) {
	// Eventually this will use a quad face, but for now we get two triangles
	polys.Add(i0, i1, i2, &vertices, this);
	polys.Add(i2, i3, i0, &vertices, this);
}

//void ott::SubObject::AddSubGeometry(Vertex* v0, Vertex* v1, Vertex* v2) {
//	addTriangle(v0, v1, v2);
//}
//
//void ott::SubObject::AddSubGeometry(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3) {
//	addTriangle(v0, v1, v2);
//	addTriangle(v2, v3, v0);
//}
