#pragma warning(disable:4996)//エラー対処

#include <cstdio>
#include <cmath>


#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

static int WIN_WIDTH = 500;                       // ウィンドウの幅
static int WIN_HEIGHT = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "2-3Rodorigues";     // ウィンドウのタイトル
static const double fps = 30.0;                     // FPS

static const double PI = 4.0 * atan(1.0);           // 円周率の定義

static float theta = 0.0f;

static const float positions[8][3] = {
	{ -1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f }
};


static const float colors[6][3] = {
	{ 1.0f, 0.0f, 0.0f },  // 赤
	{ 0.0f, 1.0f, 0.0f },  // 緑
	{ 0.0f, 0.0f, 1.0f },  // 青
	{ 1.0f, 1.0f, 0.0f },  // イエロー
	{ 0.0f, 1.0f, 1.0f },  // シアン
	{ 1.0f, 0.0f, 1.0f },  // マゼンタ
};

static const unsigned int indices[12][3] = {
	{ 1, 6, 7 }, { 1, 7, 4 },
	{ 2, 5, 7 }, { 2, 7, 4 },
	{ 3, 5, 7 }, { 3, 7, 6 },
	{ 0, 1, 4 }, { 0, 4, 2 },
	{ 0, 1, 6 }, { 0, 6, 3 },
	{ 0, 2, 5 }, { 0, 5, 3 }
};

// OpenGLの初期化関数
void initializeGL() {
	// 背景色の設定 (黒)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 深度テストの有効化
	glEnable(GL_DEPTH_TEST);
}

// キューブの描画
void drawCube() {
	glBegin(GL_TRIANGLES);
	for (int face = 0; face < 6; face++) {
		glColor3fv(colors[face]);
		for (int i = 0; i < 3; i++) {
			glVertex3fv(positions[indices[face * 2 + 0][i]]);
		}

		for (int i = 0; i < 3; i++) {
			glVertex3fv(positions[indices[face * 2 + 1][i]]);
		}
	}
	glEnd();
}

void drawCubeRodorigues(float theta,float x,float y,float z,float *mat) {
	//glRotatef(theta, 0.0f, 1.0f, 0.0f);
	float norm = sqrtf(x * x + y * y + z * z);
	x /= norm;
	y /= norm;
	z /= norm;

	float radian = theta* (PI / 360.0) ;
	mat[0] = cos(radian) + x * x*(1 - cos(radian));
	mat[1] = y * x*(1 - cos(radian)) + z * sin(radian);
	mat[2] = z * x*(1 - cos(radian)) - y * sin(radian);
	mat[4] = x * y*(1 - cos(radian)) - z * sin(radian);
	mat[5] = cos(radian) + y * y*(1 - cos(radian));
	mat[6] = z * y*(1 - cos(radian)) + x * sin(radian);
	mat[8] = x * z*(1 - cos(radian)) + y * sin(radian);
	mat[9] = y * z*(1 - cos(radian)) - x * sin(radian);
	mat[10] = cos(radian) + z * z*(1 - cos(radian));
	mat[15] = 1.0f;

	glMultMatrixf(mat);

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
	gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
		0.0f, 0.0f, 0.0f,     // 見ている先
		0.0f, 1.0f, 0.0f);    // 視界の上方向

// キューブ
	glTranslatef(0.0f, 0.0f, 0.0f);
	float mat[16] = { 0 };
	drawCubeRodorigues(theta, 1.0f, 0.0f, 0.0f,mat);
	glMultMatrixf(mat);
	glScalef(0.5f, 0.5f, 0.5f);

	drawCube();
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

	// ウィンドウのリサイズを扱う関数の登録
	glfwSetWindowSizeCallback(window, resizeGL);

	// OpenGLを初期化
	initializeGL();

	// メインループ
	double prevTime = glfwGetTime();
	while (glfwWindowShouldClose(window) == GL_FALSE) {
		double currentTime = glfwGetTime();

		// 経過時間が 1 / FPS 以上なら描画する
		if (currentTime - prevTime >= 1.0 / fps) {
			// タイトルにFPSを表示
			double realFps = 1.0 / (currentTime - prevTime);
			char winTitle[256];
			sprintf(winTitle, "%s (%.3f)", WIN_TITLE, realFps);
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
