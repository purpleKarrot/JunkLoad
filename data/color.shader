
#if defined(VERTEX_SHADER)

void main()
{
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}

#elif defined(FRAGMENT_SHADER)

void main()
{
	gl_FragColor = gl_Color;
}

#endif
