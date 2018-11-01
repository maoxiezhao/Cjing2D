#include<iostream>
#include<Windows.h>

#include"common\common.h"
#include"core\debug.h"
#include"game\app.h"
#include"core\random.h"
#include"core\inputEvent.h"
#include"utils\size.h"

#include"test.h"
#include"core\types.h"
#include"utils\vec2.h"
#include"core\resourceCache.h"
#include"core\fileData.h"
#include"utils\rectangle.h"
#include"utils\matrix4.h"

#include"core\asyncLoader.h"
#include"utils\grid.h"
#include<future>

#include "perf.h"
#include "demo.h"
#include "utils\obb.h"



int Domain();
int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);
	Debug::SetDebugStringEnable(false);
	Debug::SetDebugConsole(100, 35, true);

	//Test();
	App().Run();

	// Test Grid
	//util::Grid<int> mGrid({ 640, 480}, {320,240});
	//mGrid.Add(10, { 0,0,100,100 });

	//auto& ints = mGrid.GetElements({ 0,0,500,500 });

	//Domain();

	//system("Pause");
	return 0;
}




void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

int blowup = 0;
int screenshot = 0;
int premult = 0;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
}

//#include"perf.h"
//
//int domain()
//{
//	glfwwindow* window;
//	demodata data;
//	nvgcontext* vg = null;
//	gputimer gputimer;
//	perfgraph fps, cpugraph, gpugraph;
//	double prevt = 0, cputime = 0;
//
//	
//
//	initgraph(&fps, graph_render_fps, "frame time");
//	initgraph(&cpugraph, graph_render_ms, "cpu time");
//	initgraph(&gpugraph, graph_render_ms, "gpu time");
//
//	glfwseterrorcallback(errorcb);
//#ifndef _win32 // don't require this on win32, and works with more cards
//	glfwwindowhint(glfw_context_version_major, 3);
//	glfwwindowhint(glfw_context_version_minor, 2);
//	glfwwindowhint(glfw_opengl_forward_compat, gl_true);
//	glfwwindowhint(glfw_opengl_profile, glfw_opengl_core_profile);
//#endif
//	glfwwindowhint(glfw_opengl_debug_context, 1);
//
//#ifdef demo_msaa
//	glfwwindowhint(glfw_samples, 4);
//#endif
//	window = glfwcreatewindow(1000, 600, "nanovg", null, null);
//	//window = glfwcreatewindow(1000, 600, "nanovg", glfwgetprimarymonitor(), null);
//	if (!window) {
//		glfwterminate();
//		return -1;
//	}
//
//	glfwsetkeycallback(window, key);
//
//	glfwmakecontextcurrent(window);
//
//	glewexperimental = gl_true;
//	glewinit();
//	glviewport(0, 0, 1000, 600);
//	glenable(gl_blend);
//	glblendfunc(gl_src_alpha, gl_one_minus_src_alpha);
//
//#ifdef demo_msaa
//	vg = nvgcreategl3(nvg_stencil_strokes | nvg_debug);
//#else
//	vg = nvgcreategl3( nvg_stencil_strokes | nvg_debug);
//#endif
//	if (vg == null) {
//		printf("could not init nanovg.\n");
//		return -1;
//	}
//
//	if (loaddemodata(vg, &data) == -1)
//		return -1;
//
//	glfwswapinterval(0);
//
//	initgputimer(&gputimer);
//
//	glfwsettime(0);
//	prevt = glfwgettime();
//
//	while (!glfwwindowshouldclose(window))
//	{
//		double mx, my, t, dt;
//		int winwidth, winheight;
//		int fbwidth, fbheight;
//		float pxratio;
//		float gputimes[3];
//		int i, n;
//
//		t = glfwgettime();
//		dt = t - prevt;
//		prevt = t;
//
//		startgputimer(&gputimer);
//
//		glfwgetcursorpos(window, &mx, &my);
//		glfwgetwindowsize(window, &winwidth, &winheight);
//		glfwgetframebuffersize(window, &fbwidth, &fbheight);
//		// calculate pixel ration for hi-dpi devices.
//		pxratio = (float)fbwidth / (float)winwidth;
//
//		// update and render
//		glviewport(0, 0, fbwidth, fbheight);
//		if (premult)
//			glclearcolor(0,0,0,0);
//		else
//			glclearcolor(0.3f, 0.3f, 0.32f, 1.0f);
//		glclear(gl_color_buffer_bit|gl_depth_buffer_bit|gl_stencil_buffer_bit);
//
//		nvgbeginframe(vg, winwidth, winheight, pxratio);
//
//		renderdemo(vg, mx,my, winwidth,winheight, t, blowup, &data);
//
//		rendergraph(vg, 5,5, &fps);
//		rendergraph(vg, 5+200+5,5, &cpugraph);
//		if (gputimer.supported)
//			rendergraph(vg, 5+200+5+200+5,5, &gpugraph);
//
//		nvgendframe(vg);
//
//		// measure the cpu time taken excluding swap buffers (as the swap may wait for gpu)
//		cputime = glfwgettime() - t;
//
//		updategraph(&fps, dt);
//		updategraph(&cpugraph, cputime);
//
//		// we may get multiple results.
//		n = stopgputimer(&gputimer, gputimes, 3);
//		for (i = 0; i < n; i++)
//			updategraph(&gpugraph, gputimes[i]);
//
//		if (screenshot) {
//			screenshot = 0;
//			savescreenshot(fbwidth, fbheight, premult, "dump.png");
//		}
//
//		glfwswapbuffers(window);
//		glfwpollevents();
//	}
//
//	freedemodata(vg, &data);
//
//	nvgdeletegl3(vg);
//
//	printf("average frame time: %.2f ms\n", getgraphaverage(&fps) * 1000.0f);
//	printf("          cpu time: %.2f ms\n", getgraphaverage(&cpugraph) * 1000.0f);
//	printf("          gpu time: %.2f ms\n", getgraphaverage(&gpugraph) * 1000.0f);
//
//	glfwterminate();
//
//	system("pause");
//	return 0;
//}
