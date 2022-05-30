#version 400 core
in vec3 fNormal;
in vec3 fPosition;
out vec4 fColor;
void
main()
{
    // Materials
    vec3 Kd = vec3(1.0);
    vec3 Ks = vec3(0.0);
    float n = 128;

    // Light is at camera position
    vec3 lightPosition = vec3(0,0,0);
    vec3 lightIntensity = vec3(1.0);
   
    // Local model
    vec3 nLightDirection = normalize(lightPosition-fPosition);
    vec3 nViewDirection = normalize(vec3(0.0)-fPosition);
    vec3 nNormal = normalize(fNormal);
    
    float diffuse = dot(nNormal, nLightDirection);
    if (diffuse > 0.0)
    {
        vec3 reflectL = normalize(-nLightDirection+2.0*nNormal*dot(nNormal,nLightDirection));
        float specular = pow(max(0.0, dot(nViewDirection, reflectL)), n);

        fColor = vec4(vec3((Kd * diffuse + Ks * specular) * lightIntensity), 1.0);
    }
    else
    {
        // Shadow regions (marked in red for now)
        fColor = vec4(0.1, 0.0, 0.0, 1);
    }
}