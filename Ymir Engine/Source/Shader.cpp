#include "Shader.h"
#include "Log.h"
#include "Globals.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "External/mmgr/mmgr.h"

// Static map to keep track of the already loaded shaders in the engine
std::map<std::string, Shader*> Shader::loadedShaders;

// Static map to convert uniform type to string
std::map<UniformType, std::string> Shader::uniformTypeToString {

	{UniformType::boolean, "bool"},
	{UniformType::f1, "float"},
	{UniformType::f2, "vec2"},
	{UniformType::f3, "vec3"},
	{UniformType::f4, "vec4"},
	{UniformType::i1, "int"},
	{UniformType::i2, "ivec2"},
	{UniformType::i3, "ivec3"},
	{UniformType::i4, "ivec4"},

};

Shader::Shader()
{
	shaderProgram = 0;
	selected = false;
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	shaderProgram = 0;
	LoadShader(vertexShaderPath, fragmentShaderPath);
}

Shader::~Shader()
{
	ClearShader();
}

void Shader::LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	shaderProgram = glCreateProgram();

	if (shaderProgram == 0) {

		LOG("[ERROR] Could not create shader program.");
		return;
	}
	else {

		LOG("Shader program created successfully.");

	}

	std::string vs, fs;

	vs = ReadShaderFile(vertexShaderPath);
	AddShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	fs = ReadShaderFile(fragmentShaderPath);
	AddShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (success == 0) {

		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);

		LOG("[ERROR] Could not link shader program: '%s'\n", errorLog);
		return;
	}
	else {

		LOG("Shader program linked successfully.");

	}

	glValidateProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);

	if (!success) {

		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);

		LOG("[ERROR] Invalid shader program: '%s\n", errorLog);
		return;
	}
	else {

		LOG("Successfully loaded shader.");

	}

}

void Shader::LoadShader(const std::string& shaderFilePath)
{
	// Store the path of the shader
	path = shaderFilePath;

	shaderProgram = glCreateProgram();

	if (shaderProgram == 0) {

		LOG("[ERROR] Could not shader program.");
		return;
	}
	else {

		LOG("Shader program created successfully.");

	}

	// Retrieve the shaderString
	std::string shaderString = ReadShaderFile(shaderFilePath);

	// Function to read the uniforms directly from shader code
	ExtractUniformsFromShaderCode(shaderString);

	// Define regex objects to match specific patterns in the shader string
	std::smatch match;

	// Strings to store extracted components of the shader
	std::string version, vertexShaderCode, geometryShaderCode, fragmentShaderCode;

	/* Use regex search to extract the components of the shader and
	store them in the respective strings */

	if (std::regex_search(shaderString, match, versionRegex)) {
		version = match.str();
	}

	if (std::regex_search(shaderString, match, vertexShaderRegex)) {
		vertexShaderCode = match[1];
	}

	if (std::regex_search(shaderString, match, geometryShaderRegex)) {
		geometryShaderCode = match[1];
	}

	if (std::regex_search(shaderString, match, fragmentShaderRegex)) {
		fragmentShaderCode = match[1];
	}

	/* Concatenate the version string with the code string and add the respective
	shader to the shader program using the AddShader function */

	std::string vs = version + vertexShaderCode;
	AddShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	if (!geometryShaderCode.empty()) {

		std::string gs = version + geometryShaderCode;
		AddShader(shaderProgram, gs.c_str(), GL_GEOMETRY_SHADER);

	}

	std::string fs = version + fragmentShaderCode;
	AddShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (success == 0) {

		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);

		LOG("[ERROR] Could not link shader program: '%s'\n", errorLog);
		return;
	}
	else {

		LOG("Shader program linked successfully.");

	}

	glValidateProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);

	if (!success) {

		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);

		LOG("[ERROR] Invalid shader program: '%s\n", errorLog);
		return;
	}
	else {

		LOG("Successfully loaded shader.");

	}

	// We add the shader and its path to the map of loaded shaders 
	loadedShaders[path] = this;

}

void Shader::LoadShaderFromString(const std::string& shaderString)
{
	shaderProgram = glCreateProgram();

	if (shaderProgram == 0) {

		LOG("[ERROR] Could not create shader program.");
		return;
	}
	else {

		LOG("Shader program created successfully.");

	}

	// Define regex objects to match specific patterns in the shader string
	std::smatch match;

	// Strings to store extracted components of the shader
	std::string version, vertexShaderCode, fragmentShaderCode;

	/* Use regex search to extract the components of the shader and 
	store them in the respective strings */

	if (std::regex_search(shaderString, match, versionRegex)) { 
		version = match.str(); 
	}

	if (std::regex_search(shaderString, match, vertexShaderRegex)) {
		vertexShaderCode = match[1];
	}

	if (std::regex_search(shaderString, match, fragmentShaderRegex)) {
		fragmentShaderCode = match[1];
	}

	/* Concatenate the version string with the code string and add the respective 
	shader to the shader program using the AddShader function */

	std::string vs = version + vertexShaderCode;
	AddShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	std::string fs = version + fragmentShaderCode;
	AddShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (success == 0) {

		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);

		LOG("[ERROR] Could not link shader program: '%s'\n", errorLog);
		return;
	}
	else {

		LOG("Shader program linked successfully.");

	}

	glValidateProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);

	if (!success) {

		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);

		LOG("[ERROR] Invalid shader program: '%s\n", errorLog);
		return;
	}
	else {

		LOG("Successfully loaded shader.");

	}
}

bool Shader::IsValid()
{
	return (shaderProgram != 0);
}

void Shader::UseShader(bool toggle)
{
	if (toggle) {

		glUseProgram(shaderProgram);

	}
	else {

		glUseProgram(0);

	}

	// If the shader has editable uniforms, bind them with the engine to be able to modify the values.
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		BindUniform(&uniforms[i]);
	}

}

void Shader::ClearShader()
{
	for (auto& it = uniforms.begin(); it != uniforms.end(); ++it) 
	{
		(*it).ClearUniform();
	}
	ClearVec(uniforms);

	glUseProgram(0);

	if (shaderProgram != 0) {

		glDeleteProgram(shaderProgram);
		shaderProgram = 0;

		LOG("Successfully cleared shader");

	}

}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::SetFloat3(const std::string& name, float3 value) const
{
	glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, value.ptr());
}

void Shader::SetMatrix4x4(const std::string& name, float4x4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_TRUE, value.ptr());
}

void Shader::SetSampler2D(const std::string& name, GLuint unit) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), unit);
}

void Shader::Translate(float3 translation)
{
	this->translation = translation;
}

void Shader::Rotate(float3 rotation)
{
	this->rotation = rotation;
}

void Shader::Scale(float3 scale)
{
	this->scale = scale;
}


void Shader::SetShaderUniforms(float4x4* matrix, bool isSelected)
{
	// Set shader uniforms that are essential (matrices, time, etc.):

	float4x4 projection;
	glGetFloatv(GL_PROJECTION_MATRIX, projection.ptr());
	this->SetMatrix4x4("projection", projection.Transposed()); // Note: Transpose the matrix when passing to shader
	this->projection = projection;

	float4x4 view;
	glGetFloatv(GL_MODELVIEW_MATRIX, view.ptr());
	this->SetMatrix4x4("view", view.Transposed()); // Note: Transpose the matrix when passing to shader
	this->view = view;

	float3 pos, sc;
	Quat rot;
	matrix->Decompose(pos, rot, sc);
	float3 eulerRotation = rot.ToEulerXYZ();
	eulerRotation *= RADTODEG;
	translationMatrix = CreateTranslationMatrix(pos);
	rotationMatrix = CreateRotationMatrix(eulerRotation);
	scaleMatrix = CreateScaleMatrix(sc);

	float4x4 model = float4x4::identity;
	model = model * translationMatrix * rotationMatrix * scaleMatrix;
	this->SetMatrix4x4("model", model);
	this->model = model;

	// Selected uniform management
	this->SetBool("selected", isSelected);

	// Time uniform management
	this->SetFloat("time", TimeManager::graphicsTimer.ReadSec());

	// Game Camera Position
	this->SetUniformValue("camPos", External->scene->gameCameraComponent->mOwner->mTransform->GetGlobalPosition().ptr());

	// ----------------- Multiple Light Management ----------------- 

	this->SetInt("numLights", External->lightManager->lights.size());

	// Point Lights

	std::vector<Light*> pointLights;

	for (auto& it = External->lightManager->lights.begin(); it != External->lightManager->lights.end(); ++it)
	{
		if ((*it)->GetType() == LightType::POINT_LIGHT && (*it)->lightGO->active && (*it)->lightGO->GetComponent(ComponentType::LIGHT)->active) 
		{
			pointLights.push_back((*it));
		}
	}

	this->SetInt("numPointLights", pointLights.size());

	for (size_t i = 0; i < pointLights.size(); ++i)
	{
		std::string lightDir = std::string("pointLights[") + std::to_string(i) + std::string("].lightDir");
		this->SetFloat3(lightDir, pointLights[i]->lightGO->mTransform->GetGlobalPosition());
		this->SetUniformValue("lightDir", pointLights[i]->lightGO->mTransform->GetGlobalPosition().ptr());

		std::string lightInt = std::string("pointLights[") + std::to_string(i) + std::string("].lightInt");
		this->SetFloat(lightInt, pointLights[i]->GetIntensity());
		this->SetFloat("lightInt", pointLights[i]->GetIntensity());

		std::string lightColor = std::string("pointLights[") + std::to_string(i) + std::string("].lightColor");
		this->SetFloat3(lightColor, pointLights[i]->GetColor());
		this->SetUniformValue("lightColor", pointLights[i]->GetColor().ptr());
	}

	// Directional Lights (TODO: FRANCESC)

	// Spot Lights (TODO: FRANCESC)

	// Area Lights (TODO: FRANCESC)

	// Baked Lights

	this->SetInt("map", (int)External->scene->currentMap);
	this->SetBool("enableLighting", External->editor->gl_Lighting);
	
}

void Shader::AddShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
{
	GLuint ShaderObj = glCreateShader(shaderType);

	if (ShaderObj == 0) {

		LOG("[ERROR] Could not create shader type %d\n", shaderType);
		return;
	}

	const GLchar* p[1];
	p[0] = pShaderText;

	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {

		GLchar InfoLog[1024];

		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);

		LOG("[ERROR] Could not compile shader type %d: '%s'\n", shaderType, InfoLog);
		return;
	}

	glAttachShader(shaderProgram, ShaderObj);
}

std::string Shader::ReadShaderFile(const std::string& filename) {

	std::ifstream file;

	file.open(filename);

	if (!file.is_open()) {

		LOG("[ERROR] Unable to open file.");

	}

	std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	file.close();

	return fileContents;
}

float4x4 Shader::CreateTranslationMatrix(float3 translation)
{
	float4x4 translationMatrix = {

		1, 0, 0, translation.x,
		0, 1, 0, translation.y,
		0, 0, 1, translation.z,
		0, 0, 0, 1

	};

	return translationMatrix;
}

float4x4 Shader::CreateRotationMatrix(float3 rotation)
{
	Quat rotationQuaternion = Quat::FromEulerXYZ(DEGTORAD * rotation.x, DEGTORAD * rotation.y, DEGTORAD * rotation.z);

	rotationQuaternion.Normalize();

	float4x4 rotationMatrix = rotationQuaternion.ToFloat4x4();

	return rotationMatrix;
}

float4x4 Shader::CreateScaleMatrix(float3 scale)
{
	float4x4 scaleMatrix = {

		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1

	};

	return scaleMatrix;
}

// --------------------------------------- Uniform Management ---------------------------------------

// Add Uniform to the vector of uniforms of the shader
void Shader::AddUniform(std::string name, std::shared_ptr<void> value, UniformType type, int nElements)
{
	// First check if a uniform with the same name already exists:
	for (Uniform& existingUniform : uniforms) {

		if (existingUniform.name == name) {
			
			// If already exists, update the existing uniform with the new information
			existingUniform.value = value;
			existingUniform.type = type;
			existingUniform.nElements = nElements;

			return;
		}

	}

	// If no existing uniform with the same name is found, add a new one:

	uniforms.push_back(Uniform(name, std::move(value), type, nElements));

}

// Delete Uniform from the vector of uniforms of the shader
void Shader::DeleteUniform(std::string name)
{
	for (auto& it = uniforms.begin(); it != uniforms.end(); ++it)
	{
		if (it->name == name) {

			uniforms.erase(it);

		}

	}

}

// Function to link code uniforms to shader uniforms  
void Shader::BindUniform(Uniform* uniform) 
{
	// First get the uniform location from the shader program:
	int uniformLocation = glGetUniformLocation(shaderProgram, uniform->name.c_str());

	// Then bind the uniform value according to the uniform type:
	switch (uniform->type) {

		case UniformType::boolean:

			glUniform1i(uniformLocation, *(bool*)uniform->value.get());
			break;

		case UniformType::f1:

			glUniform1f(uniformLocation, *(GLfloat*)uniform->value.get());
			break;

		case UniformType::f1v:

			glUniform1fv(uniformLocation, uniform->nElements, (GLfloat*)uniform->value.get());
			break;

		case UniformType::i1:

			glUniform1i(uniformLocation, *(GLint*)uniform->value.get());
			break;

		case UniformType::i1v:

			glUniform1iv(uniformLocation, uniform->nElements, (GLint*)uniform->value.get());
			break;

		case UniformType::f2:

			glUniform2f(uniformLocation, *(GLfloat*)uniform->value.get(), *((GLfloat*)uniform->value.get() + 1));
			break;

		case UniformType::f2v:

			glUniform2fv(uniformLocation, uniform->nElements, (GLfloat*)uniform->value.get());
			break;

		case UniformType::i2:

			glUniform2i(uniformLocation, *(GLint*)uniform->value.get(), *((GLint*)uniform->value.get() + 1));
			break;

		case UniformType::i2v:

			glUniform2iv(uniformLocation, uniform->nElements, (GLint*)uniform->value.get());
			break;

		case UniformType::f3:

			glUniform3f(uniformLocation, *(GLfloat*)uniform->value.get(), *((GLfloat*)uniform->value.get() + 1), *((GLfloat*)uniform->value.get() + 2));
			break;

		case UniformType::f3v:

			glUniform3fv(uniformLocation, uniform->nElements, (GLfloat*)uniform->value.get());
			break;

		case UniformType::i3:

			glUniform3i(uniformLocation, *(GLint*)uniform->value.get(), *((GLint*)uniform->value.get() + 1), *((GLint*)uniform->value.get() + 2));
			break;

		case UniformType::i3v:

			glUniform3iv(uniformLocation, uniform->nElements, (GLint*)uniform->value.get());
			break;

		case UniformType::f4:

			glUniform4f(uniformLocation, *(GLfloat*)uniform->value.get(), *((GLfloat*)uniform->value.get() + 1), *((GLfloat*)uniform->value.get() + 2), *((GLfloat*)uniform->value.get() + 3));
			break;

		case UniformType::f4v:

			glUniform4fv(uniformLocation, uniform->nElements, (GLfloat*)uniform->value.get());
			break;

		case UniformType::i4:

			glUniform4i(uniformLocation, *(GLint*)uniform->value.get(), *((GLint*)uniform->value.get() + 1), *((GLint*)uniform->value.get() + 2), *((GLint*)uniform->value.get() + 3));
			break;

		case UniformType::i4v:

			glUniform4iv(uniformLocation, uniform->nElements, (GLint*)uniform->value.get());
			break;

		case UniformType::f2mat:

			glUniformMatrix2fv(uniformLocation, uniform->nElements, false, (GLfloat*)uniform->value.get());
			break;

		case UniformType::f3mat:

			glUniformMatrix3fv(uniformLocation, uniform->nElements, false, (GLfloat*)uniform->value.get());
			break;

		case UniformType::f4mat:

			glUniformMatrix4fv(uniformLocation, uniform->nElements, false, (GLfloat*)uniform->value.get());
			break;

	}

}

// Function to dynamically read the uniforms directly from shader code
void Shader::ExtractUniformsFromShaderCode(const std::string& shaderCode)
{
	// 1. We create a string stream to iterate through each line of the shader code
	std::istringstream iss(shaderCode);
	std::string line;

	// 2. Then we iterate through each line in the shader code
	while (std::getline(iss, line)) {

		// 3. We create another string stream to tokenize each line
		std::istringstream lineStream(line);
		std::string token;

		// 4. Then we iterate through tokens in the current line
		while (lineStream >> token) {

			// 5. Check if the token is "uniform"
			if (token == "uniform") {

				// 6. Read the type and name of the uniform
				std::string type, name;
				lineStream >> type >> name;

				// Remove trailing semicolon from the name
				if (!name.empty() && name.back() == ';') {
					name.pop_back();
				}

				// Ignore some uniforms because they are managed internally
				if (std::find(uniformNamesToIgnore.begin(), uniformNamesToIgnore.end(), name) != uniformNamesToIgnore.end()) {
					continue;
				}

				// 7. Extract type and name information and add uniform to the shader:
				if (type == "int") {

					std::unique_ptr<int> value = std::make_unique<int>(1);
					this->AddUniform(name, std::move(value), UniformType::i1, 1);

				}
				else if (type == "float") {

					std::unique_ptr<float> value = std::make_unique<float>(1.0);
					this->AddUniform(name, std::move(value), UniformType::f1, 1);

				}
				else if (type == "bool") {

					bool isPlayerHit; 

					if (name == "playerHit") // I'm really sorry for this, but it had to be done ;w;
					{
						isPlayerHit = false;
					}
					else 
					{
						isPlayerHit = true;
					}

					std::unique_ptr<bool> value = std::make_unique<bool>(isPlayerHit);
					this->AddUniform(name, std::move(value), UniformType::boolean, 1);
					
				}
				else if (type == "vec2") {

					std::unique_ptr<float[]> value = std::make_unique<float[]>(2);

					value[0] = 0.0f;
					value[1] = 0.0f;

					this->AddUniform(name, std::move(value), UniformType::f2, 2);

				}
				else if (type == "vec3") {

					std::unique_ptr<float[]> value = std::make_unique<float[]>(3);

					value[0] = 0.0f;
					value[1] = 0.0f;
					value[2] = 0.0f;

					this->AddUniform(name, std::move(value), UniformType::f3, 3);

				}
				else if (type == "vec4") {

					std::unique_ptr<float[]> value = std::make_unique<float[]>(4);

					value[0] = 0.0f;
					value[1] = 0.0f;
					value[2] = 0.0f;
					value[3] = 0.0f;

					this->AddUniform(name, std::move(value), UniformType::f4, 4);

				}
				else if (type == "ivec2") {

					std::unique_ptr<int[]> value = std::make_unique<int[]>(2);

					value[0] = 0;
					value[1] = 0;

					this->AddUniform(name, std::move(value), UniformType::i2, 2);

				}
				else if (type == "ivec3") {

					std::unique_ptr<int[]> value = std::make_unique<int[]>(3);

					value[0] = 0;
					value[1] = 0;
					value[2] = 0;

					this->AddUniform(name, std::move(value), UniformType::i3, 3);

				}
				else if (type == "ivec4") {

					std::unique_ptr<int[]> value = std::make_unique<int[]>(4);

					value[0] = 0;
					value[1] = 0;
					value[2] = 0;
					value[3] = 0;

					this->AddUniform(name, std::move(value), UniformType::i4, 4);

				}
				else if (type == "mat4") {

					// Create a unique_ptr for a 1-dimensional array of floats
					std::unique_ptr<float[]> value = std::make_unique<float[]>(16);

					// Initialize the array
					for (int i = 0; i < 16; ++i) {
						value[i] = 0.0f;
					}

					// Add the uniform
					AddUniform(name, std::move(value), UniformType::f4mat, 16);

				}

			}

		}

	}

	// Finally, we use std::remove_if along with erase to remove uniforms not present in the shader code:
	uniforms.erase(std::remove_if(uniforms.begin(), uniforms.end(),
		[&shaderCode](const auto& uniform) {

			// Check if the structure "uniform " + "type " + "name" is present in the shader code:
			std::string uniformDeclaration = "uniform " + uniformTypeToString[uniform.type] + " " + uniform.name;

			return shaderCode.find(uniformDeclaration) == std::string::npos;
		}),
		uniforms.end());

}

// Function to manage the value of the uniform inside the shader
void Shader::SetUniformValue(const std::string& name, const void* newValue) {

	for (Uniform& uniform : uniforms) {

		if (uniform.name == name) {

			// Copy the new value to the existing memory location
			std::memcpy(uniform.value.get(), newValue, GetUniformSize(uniform.type));

			return;
		}

	}

}

// Function to get the uniform size in memory according to its type
size_t Shader::GetUniformSize(UniformType type) {

	size_t elementSize = 0;

	switch (type)
	{
	case UniformType::boolean:

		elementSize = sizeof(bool);
		break;

	case UniformType::f1:

		elementSize = sizeof(float);
		break;

	case UniformType::f2:

		elementSize = sizeof(float) * 2;
		break;

	case UniformType::f3:

		elementSize = sizeof(float) * 3;
		break;

	case UniformType::f4:

		elementSize = sizeof(float) * 4;
		break;

	case UniformType::i1:

		elementSize = sizeof(int);
		break;

	case UniformType::i2:

		elementSize = sizeof(int) * 2;
		break;

	case UniformType::i3:

		elementSize = sizeof(int) * 3;
		break;

	case UniformType::i4:

		elementSize = sizeof(int) * 4;
		break;

	}

	return elementSize;
}