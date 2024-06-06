#version 450 core

#ifdef VERTEX_SHADER

	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec3 aNormal;
	layout(location = 2) in vec2 aTexCoords;
	layout(location = 3) in ivec4 boneIds; 
	layout(location = 4) in vec4 weights;
	
	const int MAX_BONES = 100;
	const int MAX_BONE_INFLUENCE = 4;
	uniform mat4 finalBonesMatrices[MAX_BONES];

	out vec3 Position;
	out vec3 Normal;
	out vec2 TexCoords;


	uniform mat4 projection;
	uniform mat4 view;
	uniform mat4 model;

	void main()
	{
		vec4 totalPosition = vec4(0.0f);
		for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
		{
			if(boneIds[i] == -1)
				continue;
			if(boneIds[i] >=MAX_BONES)
			{
				totalPosition = vec4(aPos,1.0f);
				break;
			}
			vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
			totalPosition += localPosition * weights[i];
			vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
		}
		gl_Position = projection * view * model * totalPosition;
		Position = aPos;
		TexCoords = aTexCoords;
		Normal = aNormal; 
	}

#endif 

#ifdef FRAGMENT_SHADER

	in vec3 Position;
	in vec3 Normal;
	in vec2 TexCoords;

    out vec4 FragColor;

    uniform sampler2D texture_diffuse1;
	uniform sampler2D texture_specular;
	uniform sampler2D texture_normal;
	uniform sampler2D texture_height;
	uniform sampler2D texture_ambient;
	uniform sampler2D texture_emissive;

    uniform vec3 lightDir;
	uniform float lightInt;
	uniform vec3 lightColor;
    uniform int numLights;

    uniform bool displayNormalMap; // Boolean uniform to toggle between normal visualization and texture display
    uniform bool selected;
    
    uniform float transparency;

	uniform bool setColor;
	uniform vec3 weaponColor;
	
	uniform bool playerHit;

    vec4 DisplayNormalMap() {

        // Normalize the interpolated normal since it's interpolated across the surface
        vec3 norm = normalize(Normal);
    
        // Adjust the normalized normal to be in the range [0, 1] for visualization purposes
        vec3 color = 0.5 * (norm + vec3(1.0));

        return vec4(color, transparency);

    }

    vec4 AddOutline(vec4 mainTexture, vec4 color, float outlineWidth) {

        // Sample the neighboring pixels

        vec2 pixelSize = 1.0 / textureSize(texture_diffuse1, 0);
        vec4 leftColor = texture(texture_diffuse1, TexCoords - vec2(outlineWidth, 0) * pixelSize);
        vec4 rightColor = texture(texture_diffuse1, TexCoords + vec2(outlineWidth, 0) * pixelSize);
        vec4 topColor = texture(texture_diffuse1, TexCoords + vec2(0, outlineWidth) * pixelSize);
        vec4 bottomColor = texture(texture_diffuse1, TexCoords - vec2(0, outlineWidth) * pixelSize);

        // Check if the current pixel is on the border

        bool isBorder = any(notEqual(mainTexture, leftColor)) || any(notEqual(mainTexture, rightColor)) ||
                        any(notEqual(mainTexture, topColor)) || any(notEqual(mainTexture, bottomColor));

        // Set the outline color if it's a border pixel, otherwise use the main color

        return isBorder ? color : mainTexture;

    }

    vec2 blinnPhongDir(vec3 lightDir, float lightInt, float Ka, float Kd, float Ks, float shininess)
	{
		vec3 s = normalize(lightDir);
		vec3 v = normalize(-Position);
		vec3 n = normalize(Normal);
		vec3 h = normalize(v+s);
		float diffuse = Ka + Kd * lightInt * max(0.0, dot(n, s));
		float spec = Ks * pow(max(0.0, dot(n,h)), shininess);
		return vec2(diffuse, spec);
	}

    void main()
    {   
        if (displayNormalMap) {
    
            FragColor = DisplayNormalMap();
        }
		else if (playerHit) {
			
			vec4 mainTexture = texture(texture_diffuse1, TexCoords);
    		mainTexture.a *= transparency;
			
			FragColor = mainTexture + vec4(1.0f,0.0f,0.0f,1.0f);

		} 
        else if (bool(numLights)) {
            
            // Calculate the lighting intensity using Blinn-Phong
		    vec2 inten = blinnPhongDir(lightDir, lightInt, 0.2, 0.8, 0.3, 80.0);
    
		    // Sample the diffuse texture
		    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;

		    // Multiply the texture color with the light intensity and add ambient term
		    vec3 finalColor = textureColor * inten.x + vec3(1.0) * inten.y;

		    // Apply the light color
		    finalColor *= lightColor;

		    // Output the final color
			if (setColor) {
				FragColor = vec4(weaponColor, transparency);
			}
			else {
				FragColor = vec4(finalColor, transparency);
			}
		    

            //if (selected) {

                //FragColor = AddOutline(vec4(finalColor, transparency), vec4(1.0, 0.5, 0.0, transparency), 0.1);

            //}

        }
        else {

            vec4 mainTexture = texture(texture_diffuse1, TexCoords);
    		mainTexture.a *= transparency;
    		
            FragColor = mainTexture;

            //if (selected) {

                //FragColor = AddOutline(mainTexture, vec4(1.0, 0.5, 0.0, transparency), 0.2);

            //}

        }
    }

#endif













