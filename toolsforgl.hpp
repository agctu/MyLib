vec3 RainbowColor(float index)
{
//A function used to get the seven color from the rainbow
//A float between [0,1] is required,and a vec3 present the color is returned
//returned color is smoothed by linear insertion
	index *= 6;
	vec3 rain[7] = {
		vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.5f, 0.0f), vec3(1.0f, 1.0f, 0.0f), 
		vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f),
		vec3(0.0f, 0.0f ,1.0f), vec3(1.0f, 0.0f, 1.0f)};
	if ((int)index == index)
		return rain[(int)index];
	else {
		int left = floor(index), right = ceil(index);
		return (right - index) * rain[left] + (index - left) * rain[right];
	}

}
class Viewchangebydrag {
	mat4 accumulated;
	mat4 current;
	bool press;
	vec2 pressed;
public:
	vec2 cursorPos;
	Viewchangebydrag():press(false){}
	void mouse_button(int button,int action)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			press = true;
			pressed = cursorPos;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			press = false;
			accumulated = current * accumulated;
			current = mat4();
			pressed = vec2(0.0f);
		}
	}
	void mouse_move(int x, int y)
	{
		cursorPos = vec2((float)x, (float)y);
		if (press)
			current = rotate(mat4(), radians(length(cursorPos - pressed)), vec3((cursorPos - pressed).y, (cursorPos - pressed).x,0.0f));
	}
	mat4 viewmat()
	{
		return current * accumulated;
	}
};
void framemanage(int desirerate)
{
//this function help adjust the frames per second,just by suspend the process
	static double time = 0.0;
	static double lasttime = 0.0;
	static int length = 0;
	static int count = 0;
	time = glfwGetTime();
	double framerate = 1.0 / (time - lasttime);
	char logdata[512];
	sprintf(logdata, "%.4lf    per second", framerate);
	/*if (count == desirerate / 6.0 * 3.0) {
		frame.holdoutput(logdata);
		count = 0;
	}*/
	if (framerate < desirerate && length - 1 > 0)
		length--;
	if (framerate > desirerate)
		length++;
	lasttime = time;
	count++;

	//posLogdisplay
	/*{
		posLog.setup();
		static vec3 lastPos;
		if (Pos != lastPos) {
			sprintf(logdata, "Pos.x:   %f,   Pos.y:     %f,     Pos.z:     %f", Pos.x, Pos.y, Pos.z);
			posLog.holdoutput(logdata);
			lastPos = Pos;
		}
	}*/
}