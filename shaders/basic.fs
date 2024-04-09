#version 330

uniform vec4 lumpos;

uniform sampler2D myTexture;
uniform int hasTexture;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform vec4 ambient_color;
uniform vec4 emission_color;
uniform float shininess;

in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;

out vec4 fragColor;

void main(void) {
    vec3 lum  = normalize(vsoModPosition.xyz - lumpos.xyz);
    float diffuse = clamp(dot(normalize(vsoNormal), -lum), 0.0, 1.0);
    vec3 lightDirection = vec3(lumpos - vsoModPosition);
    
    // 기존의 조명 계산 로직
    vec4 specularReflection = specular_color * pow(max(0.0, dot(normalize(reflect(-lightDirection, vsoNormal)), normalize(vec3(-vsoModPosition)))), shininess);
    vec4 diffuseReflection = ambient_color*0.2 + diffuse_color * diffuse;
    fragColor = diffuseReflection + specularReflection;

    if(hasTexture != 0) {
        fragColor *= texture(myTexture, vsoTexCoord);
    }

    // 조명 방향과 표면 노멀 사이의 각도에 따라 색상 조정
    if(diffuse < 0.2) { // 태양을 직접 바라보지 않는 경우
           fragColor *= vec4(20.0, 20.0, 20.0, 20.0); // 전체 색상을 약간 어둡게 조정
       }
}

