#include <cstdio>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<time.h>

#include <iostream>//実行結果出力に必要(cout)
#include <Windows.h>//time.hはミリ秒まで、マイクロ秒の時間とるのに必要
#pragma comment(lib,"winmm.lib")//付属品

static int WIN_WIDTH = 500;                       // ウィンドウの幅
static int WIN_HEIGHT = 500;                       // ウィンドウの高さ
static const char* WIN_TITLE ="4-2.vertex-buffer";     // ウィンドウのタイトル

// 頂点オブジェクト
struct Vertex {
	Vertex(const glm::vec3& position_, const glm::vec3& color_)
		: position(position_)
		, color(color_) {
	}

	glm::vec3 position;
	glm::vec3 color;
};

static const glm::vec3 positions[8] = {
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f,  1.0f)
};

static const glm::vec3 colors[6] = {
	glm::vec3(1.0f, 0.0f, 0.0f),  // 赤
	glm::vec3(0.0f, 1.0f, 0.0f),  // 緑
	glm::vec3(0.0f, 0.0f, 1.0f),  // 青
	glm::vec3(1.0f, 1.0f, 0.0f),  // イエロー
	glm::vec3(0.0f, 1.0f, 1.0f),  // シアン
	glm::vec3(1.0f, 0.0f, 1.0f),  // マゼンタ
};

static const unsigned int faces[12][3] = {
	{ 1, 6, 7 },{ 1, 7, 4 },
	{ 2, 5, 7 },{ 2, 7, 4 },
	{ 3, 5, 7 },{ 3, 7, 6 },
	{ 0, 1, 4 },{ 0, 4, 2 },
	{ 0, 1, 6 },{ 0, 6, 3 },
	{ 0, 2, 5 },{ 0, 5, 3 }
};

// バッファを参照する番号
GLuint vertexBufferId;
GLuint indexBufferId;

// OpenGLの初期化関数
void initializeGL() {
	// 深度テストの有効化
	glEnable(GL_DEPTH_TEST);

	// 背景色の設定 (黒)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 頂点配列の作成
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int idx = 0;
	for (int face = 0; face < 6; face++) {
		for (int i = 0; i < 3; i++) {
			vertices.push_back(Vertex(positions[faces[face * 2 + 0][i]], colors[face]));
			indices.push_back(idx++);
		}

		for (int i = 0; i < 3; i++) {
			vertices.push_back(Vertex(positions[faces[face * 2 + 1][i]], colors[face]));
			indices.push_back(idx++);
		}
	}

	// 頂点バッファの作成
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);

	// 頂点番号バッファの作成
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);
}

// OpenGLの描画関数
void paintGL() {
	// 背景色の描画
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 座標の変換
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(24.0f, 32.0f, 40.0f,     // 視点の位置
		0.0f, 0.0f, 0.0f,     // 見ている先
		0.0f, 1.0f, 0.0f);    // 視界の上方向

	 // 頂点バッファの有効化
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

	// 立方体の描画
	for (int num = 0; num < 1000; num++) {
		int x = num % 10;
		int y = ((num - x) / 10) % 10;
		int z = (num - y * 10) / 100;

		glPushMatrix();
		glTranslatef(-3.0f*x, -3.0f*y, -3.0f*z);//直径2.0の立方体をー側に1.0ずつずらして表示
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glPopMatrix();// 毎回保存していた変換行列を復帰させる

	}
	// 頂点バッファの無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void resizeGL(GLFWwindow * window, int width, int height) {
	// ユーザ管理のウィンドウサイズを変更
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	// GLFW管理のウィンドウサイズを変更
	glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

	// 実際のウィンドウサイズ (ピクセル数) を取得
	int renderBufferWidth, renderBufferHeight;
	glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

	// ビューポート変換の更新
	glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

int main(int argc, char** argv) {
	// OpenGLを初期化する
	if (glfwInit() == GL_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// Windowの作成
	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
		NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Window creation failed!");
		glfwTerminate();
		return 1;
	}

	// OpenGLの描画対象にWindowを追加
	glfwMakeContextCurrent(window);

	// OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
	const int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
		return 1;
	}

	// バージョンを出力する
	printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));


	// OpenGLを初期化
	initializeGL();

	// ウィンドウのリサイズを扱う関数の登録
	glfwSetWindowSizeCallback(window, resizeGL);

	static const int sample = 1000;
	int frame = 0;
	double time_sum = 0.0;

	//メインループ
	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		if (frame < sample) {
			LARGE_INTEGER freq;//Windows.hを使うと使える型。doubleよりも大きい()
			if (!QueryPerformanceFrequency(&freq)) {//単位を秒に直すために必要
				return 0;
			}
			LARGE_INTEGER start, end;
			if (!QueryPerformanceCounter(&start)) {
				continue;
			}
			//描画
			paintGL();
			if (!QueryPerformanceCounter(&end)) {
				continue;
			}
			//実行時間を秒単位に計算
			double time = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
			//実行時間を足す
			time_sum += time;

			std::cout << "frame= " << frame;
			std::cout << ",duration= " << time << "sec" << std::endl;
			++frame;//if文だから
		}
		else {
			//描画
			paintGL();//これがないとsample回終わったあと描画終わり、リサイズしてくれない。
		}
		// 描画用バッファの切り替え
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (frame == sample) {
			std::cout << "average duration of ";
			std::cout << sample << "times= ";
			std::cout << time_sum / sample << "sec" << std::endl;
			++frame;
		}

	}
}
