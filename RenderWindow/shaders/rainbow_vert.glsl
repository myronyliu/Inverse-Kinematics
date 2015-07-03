varying vec4 color;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;

    float cosX = dot(gl_Normal,vec3(1.0f,0.0f,0.0f));
    float cosY = dot(gl_Normal,vec3(0.0f,1.0f,0.0f));
    float cosZ = dot(gl_Normal,vec3(1.0f,0.0f,1.0f));

    /*float theX = acos(cosX);
    float theY = acos(cosY);
    float theZ = acos(cosZ);
    float r = cos(theX/2.0f);
    float g = cos(theY/2.0f);
    float b = cos(theZ/2.0f);*/
    
    float r = abs(cosX);
    float g = abs(cosY);
    float b = abs(cosZ);

    float s = 0.75f;
    color = vec4(s*r,s*g,s*b,1.0f);
}
