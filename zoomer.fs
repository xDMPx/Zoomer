#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 mousePos;
uniform vec2 screenSize;


vec3 rgbToHsv(vec3 rgb);
vec3 hsvToRgb(vec3 hsv);

void main(){
    vec4 textureColor = texture(texture0, fragTexCoord);

    vec2 mousePos = vec2(mousePos.x,mousePos.y);
    vec2 scaledCoord = vec2(fragTexCoord.x * screenSize.x, fragTexCoord.y * screenSize.y);

    float r = screenSize.x*0.1;
    float x = scaledCoord.x - mousePos.x;
    float y = scaledCoord.y - mousePos.y;

    // circle equation
    bool circle = x*x + y*y <= r*r; 

    if (!circle) {
        vec3 hsv = rgbToHsv(vec3(textureColor.r, textureColor.g, textureColor.b));
        hsv.y *= 0.25;
        vec3 rgb = hsvToRgb(hsv);
        finalColor  = vec4(rgb.x, rgb.y, rgb.z, textureColor.a);
    }
    else {
        finalColor = textureColor;
    }
}


// https://www.rapidtables.com/convert/color/rgb-to-hsv.html
vec3 rgbToHsv(vec3 rgb) {
    float r = rgb.x;
    float g = rgb.y;
    float b = rgb.z;

    float C_max = max(max(r,g),b);
    float C_min = min(min(r,g),b);
    float delta = C_max-C_min;

    float H = 0;

    if (delta != 0) {
        if (C_max == r) {
            H = mod((g-b)/delta,6);
        } else if (C_max == g) {
            H = ((b-r)/delta) + 2.0;
        } else {
            H = ((r-g)/delta) + 4.0;
        }
    }
    H*=60;

    float S = (C_max == 0 ? 0 : delta/C_max);
    float V = C_max;

    return vec3(H,S,V);
}

// https://www.rapidtables.com/convert/color/hsv-to-rgb.html
vec3 hsvToRgb(vec3 hsv) {
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float C = v*s;
    float X = C*(1-abs(mod((h/60),2)-1));
    float m = v-C;

    if (h < 60) {
        return vec3(C,X,0)+m;
    } else if (h < 120) {
        return vec3(X,C,0)+m;
    } else if (h < 180) {
        return vec3(0,C,X)+m;
    } else if (h < 240) {
        return vec3(0,X,C)+m;
    } else if (h < 300) {
        return vec3(X,0,C)+m;
    } else {
        return vec3(C,0,X)+m;
    }
}
