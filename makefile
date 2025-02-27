default:
	gcc Demos/cube.c -o cube Graphics.c ansi-terminal/Ansi.c -lm
	gcc Demos/ball.c -o ball Graphics.c ansi-terminal/Ansi.c -lm
	gcc Demos/doom.c -o doom Graphics.c ansi-terminal/Ansi.c -lm