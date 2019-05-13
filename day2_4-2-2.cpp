#include <cstdio>
#include <cmath>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>


#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>
#include <time.h>
#include <glm/glm.hpp>

#include <iostream>//実行結果出力に必要
#include <Windows.h>//time.hはミリ秒まで、マイクロ秒の時間とるのに必要
#pragma comment(lib,"winmm.lib")//付属品


static int WIN_WIDTH = 500;                 // ウィンドウの幅
static int WIN_HEIGHT = 500;                 // ウィンドウの高さ
static const char* WIN_TITLE = "4-2.glbegin";     // ウィンドウのタイトル


static const float positions[8][3] = {
	{ -1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{ 1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ 1.0f, -1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f }
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
	{ 1, 6, 7 },{ 1, 7, 4 },
	{ 2, 5, 7 },{ 2, 7, 4 },
	{ 3, 5, 7 },{ 3, 7, 6 },
	{ 0, 1, 4 },{ 0, 4, 2 },
	{ 0, 1, 6 },{ 0, 6, 3 },
	{ 0, 2, 5 },{ 0, 5, 3 }
};

// OpenGLの初期化関数
void initializeGL() {
	// 背景色の設定 (黒)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 深度テストの有効化
	glEnable(GL_DEPTH_TEST);
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
	gluLookAt(24.0f, 32.0f, 40.0f,     // 視点の位置
		0.0f, 0.0f, 0.0f,     // 見ている先
		0.0f, 1.0f, 0.0f);    // 視界の上方向

	 // 立方体の描画
	for (int  num= 0; num< 1000; num++) {
		int x = num % 10;
		int y = ((num - x) / 10) % 10;
		int z = (num - y * 10) / 100;

		/*static const float shift[3] = {
			x,y,z
		};*/

		glBegin(GL_TRIANGLES);
		for (int face = 0; face < 6; face++) {
			glColor3fv(colors[face]);
			for (int i = 0; i < 3; i++) {
				float pos[3] = {
					positions[indices[face * 2 + 0][i]][0] - 3 * x,
					positions[indices[face * 2 + 0][i]][1] - 3 * y,
					positions[indices[face * 2 + 0][i]][2] - 3 * z

				};

				glVertex3fv(pos);

				/*for (int j = 0; j < 3; j++) {
					glVertex3fv(positions[indices[face * 2 + 0][i]][j] - 3 * shift[j]);
				}*/
			}

			for (int i = 0; i < 3; i++) {
				float pos[3] = {
					positions[indices[face * 2 + 1][i]][0] - 3 * x,
					positions[indices[face * 2 + 1][i]][1] - 3 * y,
					positions[indices[face * 2 + 1][i]][2] - 3 * z

				};

				glVertex3fv(pos);

				/*for (int j = 0; j < 3; j++) {
					glVertex3fv(positions[indices[face * 2 + 1][i]][j] - 3 * shift[j]);
				}*/
			}
		}
		glEnd();
	}
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


	// ウィンドウのリサイズを扱う関数の登録
	glfwSetWindowSizeCallback(window, resizeGL);

	// OpenGLを初期化
	initializeGL();

	static const int sample = 1000;
	int frame = 0;
	double time_sum = 0.0;

	//メインループ
	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		if (frame < sample) {
			LARGE_INTEGER freq;//Windows.hを使うと使える型。doubleよりも大きい
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
			paintGL();//描画終わったあとリサイズしてくれない
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
