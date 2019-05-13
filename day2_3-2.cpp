#include <cstdio>
#include <cmath>
#include <string>

#define GLAD_GL_IMPLEMENTATION //追加
#include <glad/gl.h> //追加

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "common.h"

static int WIN_WIDTH = 500;                 // ウィンドウの幅
static int WIN_HEIGHT = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "3-2";     // ウィンドウのタイトル

static const double PI = 4.0 * atan(1.0);           // 円周率の定義

static float theta = 0.0f;

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) ;
static GLuint textureId = 0u;
static bool enableMipmap = true;

static const float positions[4][3] = {
	{ -1.0f,  0.0f, -100.0f },
	{  1.0f,  0.0f, -100.0f },
	{ -1.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  1.0f },
};

static const float texcoords[4][2] = {
	{  0.0f,  0.0f },
	{  1.0f,  0.0f },
	{  0.0f,  1.0f },
	{  1.0f,  1.0f }
};

static const unsigned int indices[2][3] = {
	{ 0, 1, 3 }, { 0, 3, 2 }
};

// OpenGLの初期化関数
void initializeGL() {
	// 背景色の設定 (黒)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 深度テストの有効化
	glEnable(GL_DEPTH_TEST);

	// テクスチャの有効化
	glEnable(GL_TEXTURE_2D);

	// テクスチャの生成と有効化
	glGenTextures(1, &textureId);

	// テクスチャの設定
	for (int i = 0; i < 4; i++) {
		int texWidth, texHeight, channels;

		std::string FILE = TEX_FILE + "level" + std::to_string(i) + ".png";

		unsigned char *bytes = stbi_load(FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);//forのなかで定義＝forの中でしか認知されない
		if (!bytes) {
			fprintf(stderr, "Failed to load image file: %s\n", FILE.c_str());
			exit(1);
		}

		glBindTexture(GL_TEXTURE_2D, textureId);

		// 単純なテクスチャの転送
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, texWidth, texHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

		// ロードした画素情報の破棄
		stbi_image_free(bytes);
	}

	// テクスチャの画素値参照方法の設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);//変えたGL_LINEARからGL_NEAREST_MIPMAP_NEAREST

	// テクスチャ境界の折り返し設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
	// テクスチャの無効化
	glBindTexture(GL_TEXTURE_2D, 0);

	
}

// OpenGLの描画関数
void paintGL() {
	// 背景色と深度値のクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 座標の変換
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1.0f, 2.0f, 6.0f,     // 視点の位置
		0.0f, 0.0f, 0.0f,     // 見ている先
		0.0f, 1.0f, 0.0f);    // 視界の上方向

	//glRotatef(theta, 0.0f, 1.0f, 0.0f);  // y軸中心にthetaだけ回転

	// 立方体の描画
	glBindTexture(GL_TEXTURE_2D, textureId);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 3; i++) {
		glTexCoord2fv(texcoords[indices[0][i]]);
		glVertex3fv(positions[indices[0][i]]);
	}

	for (int i = 0; i < 3; i++) {
		glTexCoord2fv(texcoords[indices[1][i]]);
		glVertex3fv(positions[indices[1][i]]);
	}
	glEnd();
	//	glBindTexture(GL_TEXTURE_2D, 0);
}

void resizeGL(GLFWwindow *window, int width, int height) {
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

// アニメーションのためのアップデート
void animate() {
	theta += 1.0f;  // 1度だけ回転
}

int main(int argc, char **argv) {
	// OpenGLを初期化する
	if (glfwInit() == GL_FALSE) {
		fprintf(stderr, "Initialization failed!\n");
		return 1;
	}

	// Windowの作成
	GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
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



	// ウィンドウのリサイズを扱う関数の登録
	glfwSetWindowSizeCallback(window, resizeGL);

	// OpenGLを初期化
	initializeGL();

	// メインループ
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		// 描画
		paintGL();

		// アニメーション
		animate();

		// 描画用バッファの切り替え
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
