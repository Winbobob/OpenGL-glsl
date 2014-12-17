varying vec3 N;													//Normal vector
varying vec3 v;    

void main (void)  
{  
   vec3 L = normalize(gl_LightSource[0].position.xyz - v);		//Light vector
   vec3 V = normalize(-v);										//View vector
   vec3 H = normalize(L + V);									//Half vector between L and V
 
   //calculate Ambient Term:  
   vec4 Iamb = gl_FrontLightProduct[0].ambient;    

   //calculate Diffuse Term:  
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);    
   
   // calculate Specular Term:
   vec4 Ispec = gl_FrontLightProduct[0].specular 
                * pow(max(dot(H,N),0.0),0.3*gl_FrontMaterial.shininess);

   // write Total Color:  
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;   
}