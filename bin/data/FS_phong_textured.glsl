#version 330

// get these data from the previous Shader Unit in the pipeline
in vec2 vs_texCoords;
in vec3 vs_vNormal;
in vec3 vs_vToCamera;
in vec3 vs_vToLight;

//uniform vec3 u_vCameraPositionWS;
//uniform vec3 u_vLightPositionWS;

uniform sampler2D u_texDiffuse;
uniform sampler2D u_texEmissive;

// we should use this, instead of gl_FragColor which is deprecated since GL version 3.0
out vec4 out_fragColor;

struct LightProperties
{
    vec3   vAmbiant;
    vec3   vDiffuse;
    vec3   vSpecular;
};
uniform LightProperties u_light;

struct MaterialProperties
{
    vec3   vAmbiant;
    vec3   vDiffuse;
    vec3   vSpecular;
    float  fSpecPower;
};
uniform MaterialProperties u_material;


// returns intensity of reflected ambient lighting
vec3 ambientLighting()
{
    return u_light.vAmbiant;
//    return u_material.vAmbiant * u_light.vAmbiant;
}


// returns intensity of diffuse reflection
vec3 diffuseLighting( in vec3 N, in vec3 L )
{

    // calculation as for Lambertian reflection
    float NdotL = dot( N, L );
    float diffuseTerm = clamp( NdotL, 0, 1 );

//    return u_material.vDiffuse * u_light.vDiffuse * diffuseTerm;

//    vec3 texColour = texture( u_texDiffuse, vs_texCoords ).rgb;
//    return texColour * u_light.vDiffuse * diffuseTerm;

    return u_light.vDiffuse * diffuseTerm;
}

// returns intensity of specular reflection
vec3 specularLightingBlinnPhong( in vec3 N, in vec3 L, in vec3 V )
{
   // half vector between Light and View
   vec3 H = normalize( L + V );

   float specularTerm = pow( max( dot( N, H ), 0.f ), u_material.fSpecPower );

   return u_material.vSpecular * u_light.vSpecular * specularTerm;
}

// returns intensity of specular reflection
vec3 specularLightingGouraudPhong( in vec3 N, in vec3 L, in vec3 V )
{
    // Light vector reflected by the Normal
    vec3 R = reflect( -L, N );

    float specularTerm = pow( max( dot( R, V ), 0.f ), u_material.fSpecPower );


    return u_material.vSpecular * u_light.vSpecular * specularTerm;
}



//////////////////////////////////////////////////////////////////////
void main(void)
{
   // re-normalize vectors after interpolation
    vec3 L = normalize(vs_vToLight);
    vec3 V = normalize(vs_vToCamera);
//    vec3 L = normalize(u_vLightPositionWS - vs_vPos);
//    vec3 V = normalize(u_vCameraPositionWS - vs_vPos);
    vec3 N = normalize(vs_vNormal);


   // get Blinn-Phong reflectance components
   vec3 Iamb = ambientLighting();
   vec3 Idif = diffuseLighting( N, L );
   vec3 Ispe = specularLightingGouraudPhong( N, L, V );

   // diffuse color of the object from texture
   vec3 texColour = texture( u_texDiffuse, vs_texCoords ).rgb;
//   vec3 texColour = texture( u_texDiffuse, vec2( vs_texCoords.x, 1 - vs_texCoords.y) ).rgb;


   float NdotL = dot( N, L );

   vec3 Iem = vec3(0,0,0);

   // if "in the dark", use a non null emissive term
   if( NdotL < 0 )
   {
       Iem = texture( u_texEmissive, vs_texCoords ).rgb;

       // smooth transition
       Iem *= -NdotL;
   }


//   out_fragColor.xyz = Iamb + Idif + Ispe ;
   out_fragColor.xyz = ( Iamb + Idif ) * texColour + Ispe + Iem;
   out_fragColor.a = 1;
}
