// Complex multiplication: (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
#define cx_mul(a, b) vec2((a).x * (b).x - (a).y * (b).y, (a).x * (b).y + (a).y * (b).x)

// Pure imaginary complex exponential: e^(i * theta) = cos(theta) + i * sin(theta)
#define cx_exp_i(imag) vec2(cos(imag), sin(imag))

// General complex exponential: e^(a + bi) = e^a * (cos(b) + i * sin(b))
#define cx_exp(z) (exp((z).x) * vec2(cos((z).y), sin((z).y)))

// Conjugation
#define conj(z) (vec2(z.x, -z.y))