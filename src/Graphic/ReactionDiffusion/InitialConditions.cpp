#include "./InitialConditions.hpp"

InitialConditions::InitialConditions()
{
}

InitialConditions::InitialConditions(const std::string& computeShaderPath)
{
	//changeShader(computeShaderPath);

	// DO THAT CLEANER
	std::vector<std::string> files {
		"src/shaders/initialConditions/glslSpec.comp", // Specs must be the first compiled file
		"src/shaders/initialConditions/triangle.comp",
		"src/shaders/initialConditions/hexagon.comp",
		"src/shaders/initialConditions/circle.comp",
		"src/shaders/initialConditions/initialConditionsMain.comp", // Main must be the last compiled file
	};
	_computeShader = ComputeShader(files);
}

void InitialConditions::changeShader(const std::string& computeShaderPath)
{
	try {
		_computeShader = ComputeShader(computeShaderPath);
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "[InitialConditions] cannot find compute shader: " << computeShaderPath << std::endl;
	}
}

void InitialConditions::execShader(const glm::vec2& screenDimensions)
{
	if (_computeShader.getProgramID() == 0)
		return;

	_computeShader.useProgram();

	int shapes[MAX_SHAPE];
	for (int i = 0; i < MAX_SHAPE; i++) // Reset shapes
		shapes[i] = -1;

	float radius[MAX_SHAPE];
	float borderSize[MAX_SHAPE];
	float rotationAngle[MAX_SHAPE];

	for (int i = 0; i < (int)_shapes.size(); i++)
	{
		shapes[i] = (int)_shapes[i].shape;
		radius[i] = _shapes[i].radius;
		borderSize[i] = _shapes[i].borderSize;
		rotationAngle[i] = _shapes[i].rotationAngle;
	}

	const GLuint programId = _computeShader.getProgramID();
	glUniform1fv(glGetUniformLocation(programId, "radius"), MAX_SHAPE, radius);
	glUniform1fv(glGetUniformLocation(programId, "border"), MAX_SHAPE, borderSize);
	glUniform1fv(glGetUniformLocation(programId, "angle"), MAX_SHAPE, rotationAngle);
	glUniform1iv(glGetUniformLocation(programId, "shapes"), MAX_SHAPE, shapes);

	glDispatchCompute(ceil(screenDimensions.x/8),ceil(screenDimensions.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void InitialConditions::addShape(InitialConditionsShape shape)
{
	if (_shapes.size() >= MAX_SHAPE)
		std::cerr << "[InitialConditions]: could not add shape, max shape limit reached" << std::endl;
	else
		_shapes.push_back(shape);
}

std::vector<InitialConditionsShape>& InitialConditions::getShapes()
{
	return _shapes;
}

void InitialConditions::clearShapes()
{
	_shapes.clear();
}

void InitialConditions::removeShape(const int& index)
{
	int i = 0;
	for (std::vector<InitialConditionsShape>::iterator it = _shapes.begin(); it != _shapes.end(); it++)
	{
		if (i == index)
		{
			_shapes.erase(it);
			return;
		}
		i++;
	}
}
