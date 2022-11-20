#include <iostream>
#include <array>
#include <fstream>
#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int Width = 800;
int Heigth = 600;

std::string LerArquivo(const char* caminho) {
	std::string conteudoArquivo;
	if (std::ifstream FileStream{ caminho , std::ios::in }) {
		conteudoArquivo.assign(std::istreambuf_iterator<char>(FileStream), std::istreambuf_iterator<char>());
	}
	return conteudoArquivo;
}

void CheckShader(GLuint ShaderId) {
	GLint Result = GL_TRUE;
	glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE) {
		GLint tamanhoLog = 0;
		glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &tamanhoLog);

		if (tamanhoLog > 0) {
			std::string textoLog(tamanhoLog, '\0');
			glGetShaderInfoLog(ShaderId, tamanhoLog, nullptr, &textoLog[0]);
			std::cout << "Erro no shader:" << std::endl;
			std::cout << textoLog << std::endl;
			assert(false);
		}
	}
}

GLuint LoadShaders(const char* VertexShaderFile, const char* FragmentShaderFile) {
	std::string VertexShaderSource = LerArquivo(VertexShaderFile);
	std::string FragmentShaderSource = LerArquivo(FragmentShaderFile);

	assert(!VertexShaderSource.empty());
	assert(!FragmentShaderSource.empty());

	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	const char* VertexShaderSourcePtr = VertexShaderSource.c_str();
	glShaderSource(VertexShaderId, 1, &VertexShaderSourcePtr, nullptr);
	glCompileShader(VertexShaderId);

	CheckShader(VertexShaderId);

	const char* FragmentShaderPtr = FragmentShaderSource.c_str();
	glShaderSource(FragmentShaderId, 1, &FragmentShaderPtr, nullptr);
	glCompileShader(FragmentShaderId);

	CheckShader(FragmentShaderId);

	GLuint ProgramaId = glCreateProgram();
	glAttachShader(ProgramaId, VertexShaderId);
	glAttachShader(ProgramaId, FragmentShaderId);
	glLinkProgram(ProgramaId);

	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramaId, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE) {
		GLint tamanhoLog = 0;
		glGetProgramiv(ProgramaId, GL_INFO_LOG_LENGTH, &tamanhoLog);

		if (tamanhoLog > 0) {
			std::string textoLog(tamanhoLog, '\0');
			glGetProgramInfoLog(ProgramaId, tamanhoLog, nullptr, &textoLog[0]);

			std::cout << "Erro no Program:" << std::endl;
			std::cout << textoLog << std::endl;

			assert(false);
		}
	}

	glDetachShader(ProgramaId, VertexShaderId);
	glDetachShader(ProgramaId, FragmentShaderId);

	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	return ProgramaId;
}

GLint carregarTextura(const char* caminhoTextura) {
	std::cout << "Carregando textura: " << caminhoTextura << std::endl;

	stbi_set_flip_vertically_on_load(true);

	int LarguraImagem = 0;
	int AlturaImagem = 0;
	int NumeroDeComponentes = 0;
	unsigned char* DadosTextura = stbi_load(caminhoTextura,
		&LarguraImagem,
		&AlturaImagem,
		&NumeroDeComponentes,
		3);
	assert(DadosTextura);

	GLuint TexturaId;
	glGenTextures(1, &TexturaId);

	glBindTexture(GL_TEXTURE_2D, TexturaId);

	glTexImage2D(GL_TEXTURE_2D,
		0, //Level
		GL_RGB,//Formato da imagem
		LarguraImagem,
		AlturaImagem,
		0, //Borda da imagem
		GL_RGB,//Formato de Pixel dentro do Shader
		GL_UNSIGNED_BYTE,//Formato dos dados
		DadosTextura);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(DadosTextura);

	return TexturaId;
}

struct Vertice {
	glm::vec3 Posicao;
	glm::vec3 Cor;
	glm::vec2 UV;
};

GLuint carregaGeometria() {
	std::array<Vertice, 6> Quadrado = {
		Vertice{
			glm::vec3{	-1.0f,	-1.0f,	0.0f},
			glm::vec3{	1.0f,	0.0f,	0.0f},
			glm::vec2{	0.0f,	0.0f}
		},
		Vertice{
			glm::vec3{	1.0f,	-1.0f,	0.0f},
			glm::vec3{	0.0f,	1.0f,	0.0f},
			glm::vec2{	1.0f,	0.0f}
		},
		Vertice{
			glm::vec3{	1.0f,  1.0f,	0.0f},
			glm::vec3{	1.0f,	0.0f,	0.0f},
			glm::vec2{	1.0f,	1.0f	}
		},
		Vertice{
			glm::vec3{	-1.0f,  1.0f,	0.0f},
			glm::vec3{	0.0f,	0.0f,	1.0f},
			glm::vec2{	0.0f,	1.0f	}
		},
	};
	
	std::array<glm::ivec3, 2> Indices{
		glm::ivec3{	0,  1,	3},
		glm::ivec3{	3,  1,	2},
	};

	GLuint VertexBuffer;

	glGenBuffers(1, &VertexBuffer);

	GLuint ElementBuffer = 0;
	glGenBuffers(1, &ElementBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Quadrado), Quadrado.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertice),
		reinterpret_cast<void*>(offsetof(Vertice, Cor)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice),
		reinterpret_cast<void*>(offsetof(Vertice, UV)));

	glBindVertexArray(0);

	return VAO;
}

void GeraMalhaEsfera(GLuint resolucao, std::vector<Vertice>& Vertices
	, std::vector<glm::ivec3>& Indices) {
	Vertices.clear();
	Indices.clear();

	constexpr float Pi = glm::pi<float>();
	constexpr float DoisPi = glm::two_pi<float>();

	const float InversoResolucao = 1.0f / static_cast<float>(resolucao - 1);

	for (GLuint UIndice = 0; UIndice < resolucao; ++UIndice)
	{
		const float U = UIndice * InversoResolucao;
		const float Phi = glm::mix(0.0f, DoisPi, U);

		for (GLuint VIndice = 0; VIndice < resolucao; ++VIndice)
		{
			const float V = VIndice * InversoResolucao;
			const float Theta = glm::mix(0.0f, Pi, V);

			glm::vec3 PosicaoVertice{
				glm::sin(Theta) * glm::cos(Phi),
				glm::sin(Theta) * glm::sin(Phi),
				glm::cos(Theta)
			};
			Vertice Vertice{
				PosicaoVertice,
				glm::vec3{ 1.0f,1.0f,1.0f },
				glm::vec2{ U, V}
			};
			Vertices.push_back(Vertice);
		}
	}

	for (GLuint U = 0; U < resolucao - 1; ++U) {
		for (GLuint V = 0; V < resolucao - 1; ++V) {
			GLuint P0 = U + V * resolucao;
			GLuint P1 = (U + 1) + V * resolucao;
			GLuint P2 = (U + 1) + (V + 1) * resolucao;
			GLuint P3 = U + (V + 1) * resolucao;

			Indices.push_back(glm::ivec3{ P0, P1, P3 });
			Indices.push_back(glm::ivec3{ P3, P1, P2 });
		}
	}

}

GLuint CarregarEsfera(GLuint& TotalVertices, GLuint& TotalIndices) {

	std::vector<Vertice> Vertices;
	std::vector<glm::ivec3> Triangulos;
	GeraMalhaEsfera(50, Vertices, Triangulos);

	TotalVertices = Vertices.size();
	TotalIndices = Triangulos.size() * 3;

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	GLsizeiptr size = Vertices.size() * sizeof(Vertice);
	glBufferData(GL_ARRAY_BUFFER, size, Vertices.data(), GL_STATIC_DRAW);

	GLuint ElementBuffer;
	glGenBuffers(1, &ElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	GLsizeiptr sizeIndice = TotalIndices * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndice, Triangulos.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertice),
		reinterpret_cast<void*>(offsetof(Vertice, Cor)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice),
		reinterpret_cast<void*>(offsetof(Vertice, UV)));

	glBindVertexArray(0);

	return VAO;
}

class FlyCamera {
public:

	void MoverPraFrente(float quantidade) {
		this->Location += glm::normalize(Direction) * quantidade * Speed;
	}

	void MoverPraDireita(float quantidade) {
		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		this->Location += Right * quantidade * Speed;
	}

	void OlharPara(float quantidadeEixoYyaw, float quantidadeEixoXpitch) {
		quantidadeEixoYyaw *= Sensitivity;
		quantidadeEixoXpitch *= Sensitivity;

		const glm::mat4 MatrizIdentidade = glm::identity<glm::mat4>();
		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		glm::mat4 MatrizRotacaoExioY = glm::rotate(MatrizIdentidade,
			glm::radians(quantidadeEixoYyaw),
			Up);
		glm::mat4 MatrizRotacaoExioX = glm::rotate(MatrizIdentidade,
			glm::radians(quantidadeEixoXpitch),
			Right);

		this->Up = MatrizRotacaoExioX * glm::vec4{ this->Up , 0.0f };
		this->Direction = MatrizRotacaoExioY * MatrizRotacaoExioX * glm::vec4{ this->Direction , 0.0f };
	}

	glm::mat4 GetViewProjection() const {

		glm::mat4 View = glm::lookAt(Location, Location + Direction, Up);
		glm::mat4 Projection = glm::perspective(FieldOfView, AspectRatio, Near, Far);
		return Projection * View;
	}

	float Speed = 1;
	float Sensitivity = 0.1f;

	glm::vec3 Location{ 0.0f, 0.0f, 5.0f };
	glm::vec3 Direction{ 0.0f, 0.0f, -1.0f };
	glm::vec3 Up{ 0.0f, 1.0f, 0.0f };

	float FieldOfView = glm::radians(45.0f);
	float AspectRatio = Width / Heigth;
	float Near = 0.01f;
	float Far = 1000.0f;
};

FlyCamera Camera;

glm::vec2 PosicaoAnteriorCursor;
bool habilitarMovimentoMouse = false;

void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers) {
	if (Button == GLFW_MOUSE_BUTTON_LEFT) {
		if (Action == GLFW_PRESS) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double X, Y;
			glfwGetCursorPos(Window, &X, &Y);
			PosicaoAnteriorCursor = glm::vec2{ X, Y };
			habilitarMovimentoMouse = true;
		}
		else if (Action == GLFW_RELEASE) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			habilitarMovimentoMouse = false;
		}
	}
}

void MouseMotionCallback(GLFWwindow* Window, double X, double Y) {

	if (habilitarMovimentoMouse) {
		glm::vec2 PosicaoAtualCursor{ X, Y };
		glm::vec2 Delta = PosicaoAnteriorCursor - PosicaoAtualCursor;

		Camera.OlharPara(-Delta.x, -Delta.y);

		PosicaoAnteriorCursor = PosicaoAtualCursor;
	}
}

void Resize(GLFWwindow* window, int NovaLargura, int NovaAltura) {
	int Width = NovaLargura;
	int Heigth = NovaAltura;

	Camera.AspectRatio = static_cast<float>(Width) / Heigth;
	glViewport(0, 0, Width, Heigth);
}

int main() {
	setlocale(LC_ALL, "pt_BR");
	assert(glfwInit() == GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(Width, Heigth, "Esfera - Pedro Henrique Pires Dias", nullptr, nullptr);
	assert(window);

	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MouseMotionCallback);
	glfwSetFramebufferSizeCallback(window, Resize);

	glfwMakeContextCurrent(window);

	int statusGlwInit = glewInit();
	std::cout << statusGlwInit << std::endl;
	assert(glewInit() == GLEW_OK);

	GLint GLMajorVersion = 0;
	GLint GLMinorVersion = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &GLMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &GLMinorVersion);

	std::cout << "Maior versão do OpenGL suportada:" << GLMajorVersion << std::endl;
	std::cout << "Menor versão do OpenGL suportada:" << GLMinorVersion << std::endl;

	std::cout << "Fabircante do driver de video:" << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Modelo da placa de video:" << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Versão do OpenGL:" << glGetString(GL_VERSION) << std::endl;
	std::cout << "Versão do GLS:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	Resize(window, Width, Heigth);

	GLuint ProgramaId = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

	GLuint TexturaId = carregarTextura("textures/pedra.jpg");

	GLuint QuadVAO = carregaGeometria();

	GLuint EsferaTotalVertices = 0;
	GLuint EsferaTotalIndices = 0;
	GLuint EsferaVAO = CarregarEsfera(EsferaTotalVertices, EsferaTotalIndices);
	std::cout << "Total de vertices da esfera: " << EsferaTotalVertices << std::endl;
	std::cout << "Total de indices da esfera: " << EsferaTotalIndices << std::endl;

	glm::mat4 MatrizIdentidade = glm::identity<glm::mat4>();
	glm::mat4 MatrizModel = MatrizIdentidade;

	//Definir cor de fundo
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	double  TempoAtualizadoFrameAnterior = glfwGetTime();

	glEnable(GL_CULL_FACE); // Trocar para ver o fundo da esfera glDiable(GL_CULL_FACE)
	glCullFace(GL_BACK);

	while (!glfwWindowShouldClose(window)) {
		double  TempoAtualizadoFrameAtual = glfwGetTime();
		double DeltaTime = TempoAtualizadoFrameAtual - TempoAtualizadoFrameAnterior;
		if (DeltaTime > 0) {
			TempoAtualizadoFrameAnterior = TempoAtualizadoFrameAtual;
		}
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ProgramaId);

		glm::mat4 MatrizViewProjection = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = MatrizViewProjection * MatrizModel;

		GLint ModelViewProjectionLoc = glGetUniformLocation(ProgramaId, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TexturaId);

		GLint TextureSamplerLoc = glGetUniformLocation(ProgramaId, "TextureSampler");
		glUniform1i(TextureSamplerLoc, 0);

		glBindVertexArray(EsferaVAO);

		glPointSize(10.0f);
		glLineWidth(10.0f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, EsferaTotalIndices, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		glUseProgram(0);

		glfwPollEvents();
		glfwSwapBuffers(window);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			Camera.MoverPraFrente(1.0f * DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			Camera.MoverPraFrente(-1.0f * DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			Camera.MoverPraDireita(1.0f * DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			Camera.MoverPraDireita(-1.0f * DeltaTime);
		}
	}

	glDeleteVertexArrays(1, &QuadVAO);

	glfwTerminate();
	return 0;
}
