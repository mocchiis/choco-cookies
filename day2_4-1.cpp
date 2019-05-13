#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <vector>
#include <cmath>
#include <string>

#define GLAD_GL_IMPLEMENTATION //OpenGLの新しい機能を使うためのローダを提供 
#include <glad/gl.h> 

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>//glm (C++ の座標系ベクターの取り扱いを中心とした数学系のライブラリー)を使う

#define STB_IMAGE_IMPLEMENTATION//テクスチャマッピング
#include "stb_image.h"
#include "common.h"

static int WIN_WIDTH = 500;                 // ウィンドウの幅
static int WIN_HEIGHT = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "4-1";     // ウィンドウのタイトル
static const double fps = 100.0;                     // FPS

static const double PI = 4.0 * atan(1.0);           // 円周率の定義

static float theta = 0.0f;

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "dice.png";
static GLuint textureId = 0u;
static bool enableMipmap = true;

using namespace glm;//名前空間は名前の衝突を防ぐために必要なもの.//seal全体の識別子が直接指定可能
//この中で書かれた名前は、外から呼ぶときには先頭に名前空間の修飾(::（スコープ解決演算子))をつけなければならない


//頂点オブジェクト
//クラス名：Vertex, メンバ変数：position(初期値position_)texcoord(初期値texcoords_)
struct Vertex {
	Vertex(const glm::vec3 &position_, const vec2 &texcoords_)//頂点配列の準備 
		: position(position_), texcoord(texcoords_) {
	}

	vec3 position;//この二つをメンバ変数として持つ
	vec2 texcoord;
};

//マッピング画像の頂点オブジェクト
//クラス名：Texture
struct Texture {
	Texture(const vec2 &texcoords_)
		:texcoord(texcoords_) {

	}

	vec2 texcoord;
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

static const unsigned int indices_dice[2][3] = {
	{ 0, 1, 3 },{ 0, 3, 2 }
};

static const vec2 texcoords[24] = {
	vec2(0.0f,0.66f),vec2(0.0f,0.33f),vec2(0.25f,0.66f),vec2(0.25f,0.33f),
	vec2(0.25f,0.33f),vec2(0.25f,0.66f),vec2(0.5f,0.33f),vec2(0.5f,0.66f),
	vec2(0.25f,0.33f),vec2(0.25f,0.0f),vec2(0.5f,0.33f),vec2(0.5f,0.0f),
	vec2(0.25f,1.0f),vec2(0.5f,1.0f),vec2(0.25f,0.66f),vec2(0.5f,0.66f),
	vec2(0.5f,0.66f),vec2(0.5f,0.33f),vec2(0.75f,0.66f),vec2(0.75f,0.33f),
	vec2(0.75f,0.66f),vec2(0.75f,0.33f),vec2(1.0f,0.66f),vec2(1.0f,0.33f)
};

static const unsigned int faces[12][3] = {
	{ 1, 6, 7 },{ 1, 7, 4 },
	{ 2, 5, 7 },{ 2, 7, 4 },
	{ 3, 5, 7 },{ 3, 7, 6 },
	{ 0, 1, 4 },{ 0, 4, 2 },
	{ 0, 2, 5 },{ 0, 5, 3 },
	{ 0, 1, 6 },{ 0, 6, 3 }
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
			vertices.push_back(Vertex(positions[faces[face * 2 + 0][i]], texcoords[face * 4 + indices_dice[0][i]]));//std::vectorのverticesに()内の要素を追加するメンバ関数
			indices.push_back(idx++);
		}

		for (int i = 0; i < 3; i++) {
			vertices.push_back(Vertex(positions[faces[face * 2 + 1][i]], texcoords[face * 4 + indices_dice[1][i]]));
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


	// テクスチャの有効化
	glEnable(GL_TEXTURE_2D);

	// テクスチャの設定
	int texWidth, texHeight, channels;
	unsigned char *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
	if (!bytes) {
		fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
		exit(1);
	}

	// テクスチャの生成と有効化
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);


	// テクスチャの画素値参照方法の設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// テクスチャ境界の折り返し設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// 単純なテクスチャの転送
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

	// テクスチャの無効化
	glBindTexture(GL_TEXTURE_2D, 0);

	// ロードした画素情報の破棄
	stbi_image_free(bytes);
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
	gluLookAt(3.0f, 3.0f, 3.0f,     // 視点の位置
		0.0f, 0.0f, 0.0f,     // 見ている先
		0.0f, 1.0f, 0.0f);    // 視界の上方向

	glRotatef(theta, 1.0f, -1.0f, 0.0f);  // y軸中心にthetaだけ回転

	glBindTexture(GL_TEXTURE_2D, textureId);

	// 立方体の描画//頂点バッファとデータ種類の紐づけ 
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glEnableClientState(GL_VERTEX_ARRAY);//データ種類の有効化 
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));


	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

	// 三角形の描画
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// 頂点バッファの無効化
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	theta += 1.0f * PI / 10.0f;  // 10分の1回転
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
	double prevTime = glfwGetTime();;
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		double currentTime = glfwGetTime();

		// 経過時間が 1 / FPS 以上なら描画する
		if (currentTime - prevTime >= 1.0 / fps) {
			// タイトルにFPSを表示
			double realFps = 1.0 / (currentTime - prevTime);
			char winTitle[256];
			sprintf_s(winTitle, "%s (%.3f)", WIN_TITLE, realFps);
			glfwSetWindowTitle(window, winTitle);

			// 描画
			paintGL();

			// アニメーション
			animate();

			// 描画用バッファの切り替え
			glfwSwapBuffers(window);
			glfwPollEvents();

			// 前回更新時間の更新
			prevTime = currentTime;
		}
	}
}
