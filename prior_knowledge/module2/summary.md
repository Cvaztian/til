# RESUMEN — Módulo 2: Circuitos Analógicos Esenciales (enfoque embedded)
## Preparación Embedded Software Engineer — Aeronautical SW / Joby Aviation

> **Prerrequisito:** Módulo 1 (Circuitos Lógicos) completado.  
> **Enfoque:** Interfaz analógico-digital, ADC/DAC, reguladores, ruido. Todo conectado con embedded y aviónica.

---

## 2.1 Semiconductores Fundamentales

### Silicio dopado

- **Tipo N** (fósforo, arsénico): portadores mayoritarios = **electrones** (carga negativa)
- **Tipo P** (boro, galio): portadores mayoritarios = **huecos** (carga positiva)
- Tipo N conduce mejor que tipo P: movilidad de electrones ≈ 2.8× mayor que huecos (esto explica por qué NMOS es más rápido que PMOS, visto en Módulo 1)

### Unión PN

- Difusión crea la **zona de deplexión** (depletion region)
- Campo eléctrico intrínseco → **barrera de potencial ≈ 0.7V** (Si), ≈ 0.3V (Ge)
- Para que haya corriente → superar la barrera

### Diodos

**Ecuación de Shockley:** I = I_s × (e^(V/(n×V_T)) − 1)  
- V_T ≈ 26mV a 25°C (voltaje térmico)
- Relación I-V es **exponencial**, no lineal

**Polarización:**
- **Directa (forward):** V_A > V_K + V_f → corriente fluye, caída ≈ V_f
- **Inversa (reverse):** V_K > V_A → corriente ≈ 0 (solo fuga de nA)
- **Ruptura (breakdown):** V_inverso > V_BR → corriente masiva (destruye diodo normal, modo de operación del Zener)

**Tipos de diodos y aplicaciones:**

| Tipo | V_f | Velocidad | Uso principal |
|------|-----|-----------|---------------|
| Rectificador (Si) | ~0.7V | Medio | AC→DC, fuentes |
| Zener | Diseñado para V_BR | N/A | Regulación, clamping, referencia V |
| Schottky | ~0.2-0.3V | Ultra-rápido | Fuentes switching, protección polaridad |
| LED | 1.8-3.2V (varía) | N/A | Indicadores (R = (V_GPIO − V_f) / I_f) |
| ESD (interno) | ~0.7V | Ultra-rápido | Protección de pines del micro |

**Aplicaciones en aviónica/embedded:**
- **Protección polaridad inversa:** Schottky en serie con alimentación
- **Diodo flyback:** En antiparalelo con cargas inductivas (relay, solenoide, motor). Absorbe spike de voltaje al apagar
- **Clamping:** Zener limita voltaje antes del ADC
- **OR de alimentación:** Redundancia con diodos (si una fuente falla, la otra toma el relevo)

**Cálculo de resistor para LED:**  
**R = (V_GPIO − V_f) / I_f**

| Ejemplo | V_GPIO | V_f | I_f | R calculado | R estándar |
|---------|--------|-----|-----|-------------|------------|
| LED rojo | 3.3V | 1.8V | 20mA | 75Ω | 82Ω |
| LED, R dado 330Ω | 3.3V | 2.0V | — | I=3.94mA | — |
| LED blanco 5V | 5.0V | 3.2V | 15mA | 120Ω | 120Ω |

$Nota: 0.7V es para Si estándar. Schottky ≈ 0.3V, LEDs = 1.8-3.2V, Ge ≈ 0.3V. Siempre verificar datasheet.

---

### Transistor BJT (Bipolar Junction Transistor)

**Terminales:**
- **Base (B):** Señal de control (GPIO). Recibe corriente I_B
- **Colector (C):** "Colecta" corriente de la carga. Entra la corriente grande I_C
- **Emisor (E):** "Emite" ambas corrientes a GND. I_E = I_C + I_B

**Tipos:** NPN (más común), PNP

**Regiones de operación:**

| Región | Condición | Comportamiento | Uso |
|--------|-----------|----------------|-----|
| Corte (OFF) | V_BE < 0.7V | I_C = 0 | Switch abierto |
| Activa | V_BE ≈ 0.7V, V_CE > V_CE(sat) | I_C = β × I_B | Amplificador |
| Saturación (ON) | V_BE ≈ 0.7V, V_CE ≈ 0.2V | I_C < β × I_B | Switch cerrado |

**β (beta)** = ganancia de corriente = I_C / I_B. Típico: 100-300.

**BJT como switch NPN (low-side):**
1. **I_B = I_C / β_forzado** (usar β_forzado = 10, NO el β del datasheet)
2. **R_B = (V_GPIO − 0.7V) / I_B**
3. Carga va entre VCC y Colector. Emisor a GND.

$Nota: No diseñar con β del datasheet — varía con temperatura, corriente, y entre unidades. Usar β_forzado = 10 garantiza saturación.

**Ejemplos de diseño:**

| Ejemplo | I_C | V_GPIO | β_forzado | I_B | R_B |
|---------|-----|--------|-----------|-----|-----|
| LED 1W | 350mA | 3.3V | 10 | 35mA | 68Ω |
| Relay 12V | 100mA | 3.3V | 10 | 10mA | 270Ω |
| Verificación sat. | 50mA | 3.3V | — | 2.6mA | β×I_B=520mA >> 50mA → saturado |

---

### Transistor MOSFET

**Terminales:**
- **Gate (G):** Señal de control (GPIO). Controlado por voltaje, I_gate ≈ 0 en estado estable
- **Drain (D):** Entra la corriente desde la carga. "Drenaje"
- **Source (S):** Sale la corriente a GND. "Fuente" de los electrones

**Tipos principales:**

| Tipo | Conduce cuando | Ubicación | Uso |
|------|----------------|-----------|-----|
| N-channel enhancement | V_GS > V_th | Low-side (entre carga y GND) | El más común |
| P-channel enhancement | V_GS < −|V_th| | High-side (entre VCC y carga) | Control de alimentación |

**Regiones de operación:**

| Región | Condición | Comportamiento | Uso |
|--------|-----------|----------------|-----|
| Corte | V_GS < V_th | No conduce | Switch OFF |
| **Lineal (triodo)** | V_GS > V_th, V_DS < V_GS−V_th | Resistencia R_DS(on) | **Switch ON** |
| Saturación | V_GS > V_th, V_DS > V_GS−V_th | Fuente de corriente | Amplificador |

> **⚠ TRAMPA DE ENTREVISTA:** "Saturación" en BJT = switch ON. "Saturación" en MOSFET = modo amplificador. El MOSFET como switch ON está en región **lineal/triodo**. Nombres opuestos.

**Parámetros clave del datasheet:**
- **V_th:** Voltaje threshold para empezar a conducir (1-3V típico)
- **R_DS(on):** Resistencia encendido (1mΩ-100mΩ). Menor = mejor
- **V_GS(max):** Excederlo destruye el gate (óxido delgado ~5nm)
- **Q_g:** Carga del gate. Determina velocidad de switching

**Pérdida de potencia:** P = I² × R_DS(on)

**Ejemplo:** Motor 12V/2A, MOSFET con R_DS(on) = 25mΩ  
P = (2)² × 0.025 = **0.1W** (vs 0.4W del BJT con V_CE(sat)=0.2V)

**P-channel high-side — lógica invertida:**
- Source conectado a VCC
- GPIO LOW (0V) → V_GS = 0 − VCC = negativo → **ENCENDIDO**
- GPIO HIGH (VCC) → V_GS = VCC − VCC = 0V → **APAGADO**
- En firmware: `#define POWER_ON() GPIO_CLEAR(POWER_PIN)`

**¿Por qué la carga va entre VCC y Drain (no entre Source y GND)?**  
Si la carga va entre Source y GND, la caída de voltaje en la carga sube V_Source, reduciendo V_GS efectivo (feedback negativo). Con la carga arriba, Source queda fijo a GND → V_GS = V_GPIO → máximo posible.

**MOSFET vs BJT — cuándo usar cada uno:**

| Aspecto | BJT | MOSFET |
|---------|-----|--------|
| Control | Corriente (I_B) | Voltaje (V_GS) |
| Consumo driver | Continuo (mA) | Solo switching (pF) |
| Pérdida ON | V_CE(sat) × I | I² × R_DS(on) |
| Velocidad | Rápido | Más rápido |
| Robustez gate/base | Robusto | Frágil (ESD) |
| Uso actual | Legacy, bajo costo | **Dominante en embedded** |

---

## 2.2 Amplificadores Operacionales (Op-Amps)

### Reglas de oro del op-amp ideal (con realimentación negativa)

1. **V+ = V−** (la diferencia es cero)
2. **I+ = I− = 0** (no entra corriente a las entradas)

$Nota: Solo aplican con realimentación negativa (salida conectada de vuelta a V−). Sin realimentación → op-amp satura a ±V_supply (comparador).

### Op-amp real vs ideal

| Parámetro | Ideal | Real típico |
|-----------|-------|-------------|
| Ganancia A_OL | ∞ | 10⁵ — 10⁶ |
| Z_in | ∞ | 1MΩ — 10¹²Ω |
| Z_out | 0 | 10-100Ω |
| Ancho de banda | ∞ | GBW 1-10 MHz |
| V_offset | 0 | 0.1-5 mV |

### Configuraciones fundamentales

| Config | Fórmula | Ganancia | Notas |
|--------|---------|----------|-------|
| **Inversor** | V_out = −(R_f/R_in) × V_in | −R_f/R_in | V+ a GND. Invierte señal |
| **No-inversor** | V_out = (1+R_f/R_in) × V_in | 1+R_f/R_in | V_in a V+. Ganancia ≥ 1 |
| **Buffer** | V_out = V_in | 1 | R_f=0, R_in=∞. Desacopla Z |
| **Sumador** | V_out = −R_f(V1/R1+V2/R2+...) | Pesos indep. | Base del DAC R-2R |
| **Diferencial** | V_out = (R_f/R1)(V2−V1) | R_f/R1 | CMRR, rechaza ruido común |

**Nomenclatura de resistencias:**
- **R_f** = resistencia de **feedback** (realimentación, conecta salida a V−)
- **R_in** = resistencia de **input** (entrada, entre V_in y V−)

**Filtro activo pasa-bajos (1er orden):**  
**f_c = 1 / (2π × R_f × C)**  
Capacitor en paralelo con R_f en el inversor. Es el **filtro anti-aliasing** antes del ADC.

**Método de análisis para entrevista (4 pasos):**
1. Identificar realimentación negativa (salida → V−)
2. Aplicar V+ = V− (Regla 1)
3. Aplicar I = 0 en entradas → corriente por R_in = corriente por R_f (Regla 2)
4. Resolver para V_out

**Buffer — ¿para qué si no amplifica?**
- Z_in altísima → no carga al sensor
- Z_out bajísima → alimenta al ADC sin perder voltaje
- Aplicación: sensor de alta impedancia → buffer → ADC

---

## 2.3 Conversión ADC/DAC ⭐ (Tema estrella)

### Conceptos fundamentales

**ADC:** Voltaje analógico → Número digital  
**DAC:** Número digital → Voltaje analógico

### Fórmulas clave

| Fórmula | Descripción |
|---------|-------------|
| **LSB = V_ref / 2^N** | Resolución mínima (escalón de voltaje) |
| **V = código × LSB** | Código digital → voltaje |
| **código = V_in / LSB** | Voltaje → código digital |
| **f_s ≥ 2 × f_max** | Teorema de Nyquist |
| **ENOB < N** | Bits efectivos < bits nominales (por ruido) |

### Resolución por bits (V_ref = 3.3V)

| Bits | Niveles | LSB | Uso típico |
|------|---------|-----|------------|
| 8 | 256 | 12.9 mV | Básico, control simple |
| 10 | 1,024 | 3.22 mV | Arduino |
| **12** | **4,096** | **0.806 mV** | **Cortex-M (el más común)** |
| 16 | 65,536 | 50.3 µV | Instrumentación |
| 24 | 16.7M | 0.197 µV | Audio, termocuplas |

**Ejemplos de conversión (12 bits, V_ref = 3.3V):**

| Código | Voltaje | Cálculo |
|--------|---------|---------|
| 0 | 0V | 0 × 0.806mV |
| 2048 | 1.650V | Mitad de escala |
| 3000 | 2.417V | 3000 × 0.806mV |
| 4095 | 3.2992V | Máximo (NO es 3.3V exacto) |

$Nota: Código máximo = 2^N − 1, que corresponde a V_ref − 1 LSB, no a V_ref exacto.

$Nota: "Resolución" ≠ "precisión". ADC de 16 bits con 200µV de ruido → los últimos ~3 bits son basura. ENOB (Effective Number of Bits) es lo que importa.

### Teorema de Nyquist-Shannon

**f_s ≥ 2 × f_max**

| Señal | f_max | f_s mínimo | f_s práctica |
|-------|-------|------------|--------------|
| Temperatura | 10Hz | 20 SPS | 100 SPS (5-10×) |
| Vibración | 1kHz | 2 kSPS | 10 kSPS |
| Audio humano | 20kHz | 40 kSPS | 44.1 kHz (CD) |

### Aliasing

Si f_s < 2 × f_max → aparece una **frecuencia fantasma** que no existe en la señal real.

**f_alias = |f_s − f_señal|** (frecuencia reflejada)

**El aliasing es IRREVERSIBLE después del ADC.** No hay algoritmo en firmware que pueda separarla de la señal real. "Como mezclar pintura: no se puede des-mezclar."

### Filtro anti-aliasing

- Filtro **pasa-bajos analógico** colocado **ANTES** del ADC
- Elimina frecuencias > f_s/2 antes de que lleguen al ADC
- **Filtra FRECUENCIAS, no voltajes** (señal de 3V a baja frecuencia → pasa; señal de 0.1V a alta frecuencia → bloqueada)
- **Obligatorio en aviónica certificada (DO-178C)**

### Cadena de adquisición correcta

```
Sensor → Buffer (op-amp) → Filtro anti-aliasing → ADC → Firmware (tu código C)
```

### Tipos de ADC

| Tipo | Resolución | Velocidad | Cómo funciona | Uso |
|------|-----------|-----------|---------------|-----|
| **SAR** | 8-18 bits | 10k-5M SPS | Búsqueda binaria (N pasos para N bits) | **Tu microcontrolador** |
| **Sigma-Delta** | 16-32 bits | 10-100k SPS | Oversampling masivo + filtrado | Termocuplas, audio, precisión |
| **Flash** | 4-8 bits | 100M-10G SPS | 2^N−1 comparadores en paralelo | Osciloscopios, radar, RF |

**SAR — búsqueda binaria en hardware:**
1. Compara V_in con V_ref/2. ¿Mayor? → MSB = 1. ¿Menor? → MSB = 0.
2. Ajusta rango y compara con el siguiente nivel (±V_ref/4).
3. Repite N veces. Cada paso resuelve 1 bit.
4. El DAC interno genera los voltajes de prueba, el comparador decide.
5. 12 bits = 12 comparaciones. Es O(N) en hardware.

**Guía rápida para entrevista:**
- "¿Qué ADC tiene tu Cortex-M?" → **SAR**
- "¿Para medir µV de una termocupla?" → **Sigma-Delta**
- "¿Para un osciloscopio de 1GHz?" → **Flash**

### DAC

**V_out = código × (V_ref / 2^N)**

- **R-2R Ladder:** Red de resistencias en el micro. Simple y rápido.
- **PWM como "DAC":** V_out(promedio) = V_GPIO × duty_cycle + filtro RC. Gratis en hardware.

---

## 2.4 Reguladores de Voltaje y Fuentes

### Problema fundamental

Baterías/fuentes entregan voltajes variables. El micro necesita voltaje estable (3.3V ± 5%).

### LDO (Low Dropout Regulator) — regulador lineal

**Concepto:** Resistor variable controlado que absorbe la diferencia V_in − V_out como calor.

**Fórmulas:**
- **V_dropout = V_in(min) − V_out** (margen mínimo, típico 100-300mV)
- **P_disipada = (V_in − V_out) × I_out**
- **η (eficiencia) = V_out / V_in**

| Ejemplo | V_in | V_out | I_out | P_disipada | η |
|---------|------|-------|-------|------------|---|
| 5V→3.3V | 5V | 3.3V | 100mA | 0.17W | 66% |
| 12V→3.3V | 12V | 3.3V | 500mA | **4.35W** ⚠ | 27.5% |
| LiPo→3.3V | 3.6V | 3.3V | 200mA | 0.06W | **91.7%** ✓ |

**Ventajas:** Salida ultra-limpia (bajo ruido), simple, barato, sin EMI  
**Desventajas:** Ineficiente si V_in >> V_out, solo baja voltaje

### Switching Regulator (DC-DC) — el eficiente

**Concepto:** MOSFET conmutando a alta frecuencia (100kHz-MHz) + inductor almacena/libera energía.

**Tipos:**

| Tipo | Función | Fórmula (ideal) |
|------|---------|-----------------|
| Buck | Baja voltaje | V_out = D × V_in |
| Boost | Sube voltaje | V_out = V_in / (1−D) |
| Buck-Boost | Sube o baja | Depende de topología |

D = duty cycle (0 a 1)

**Eficiencia típica:** 85-95%

**Ejemplo:** 12V→3.3V, 500mA, η=90%  
P_disipada = 0.18W (vs 4.35W del LDO → **24× menos calor**)

**Ventajas:** Eficiente, puede subir Y bajar voltaje  
**Desventajas:** Ruido (ripple, EMI), complejo, layout de PCB crítico

$Nota: "Switching siempre es mejor que LDO" es FALSO. Para alimentar un ADC de precisión, el ruido del switching corrompe mediciones.

### Arquitectura típica en aviónica

```
28V (bus avión) → Buck (28→5V, eficiente) → LDO (5→3.3V, limpio) → MCU + ADC
```

Buck para la conversión principal, LDO para limpiar el ruido. Lo mejor de ambos.

### Power Budgeting

Documentar consumo de cada componente con margen (20-50%). En aviónica (DO-178C), es documento formal.

**Modos de bajo consumo MCU:**

| Modo | Consumo típico | Qué funciona |
|------|---------------|--------------|
| Run | 30-100mA | Todo |
| Sleep | 5-15mA | CPU dormida, periféricos activos |
| Stop | 10-100µA | Solo RTC y wakeup |
| Standby | 1-5µA | Solo pin de wakeup |

---

## 2.5 Ruido e Integridad de Señal — EMI/EMC

### Definiciones

- **Ruido:** Señal eléctrica no deseada superpuesta a la señal útil
- **SNR = 20 × log₁₀(V_signal / V_noise)** [dB]. >60dB = excelente, <20dB = pobre
- **EMI:** Ruido que tu circuito **genera**
- **EMC:** Capacidad de funcionar correctamente **en presencia de EMI** y no generar EMI excesivo
- **DO-160G:** Estándar de aviación que define pruebas y niveles de EMC

**Relación SNR-ADC:** ADC de N bits tiene SNR máximo teórico = 6.02×N + 1.76 dB. Un ADC de 12 bits → ~74 dB. Si tu circuito tiene SNR de 40 dB, los últimos ~6 bits son ruido.

### 4 mecanismos de acoplamiento de ruido

| Mecanismo | Cómo se acopla | Ejemplo | Solución |
|-----------|---------------|---------|----------|
| **Conductivo** | Por cables/trazas compartidas | Motor y ADC comparten GND | Star ground, GND separados |
| **Capacitivo** | Campo eléctrico (C parásita) | Traza de clock junto a señal analógica | Separar trazas, guard traces |
| **Inductivo** | Campo magnético (Faraday) | Cable de motor induce V en cable sensor | Minimizar área de lazo, twisted pair |
| **Radiado** | Ondas EM por el aire | Inversor motor irradia, cable sensor es antena | Blindaje, cables cortos, filtros |

### Técnicas de mitigación

**1. Capacitor de desacoplo (bypass)**
- **100nF cerámico en cada pin VCC de cada IC.** Sin excepciones.
- 10µF electrolítico por rail de alimentación
- Lo más cerca posible del pin (milímetros, no centímetros)
- Actúa como reserva local de carga para transitorios de corriente

**2. Ground plane**
- Capa entera de cobre dedicada a GND
- Baja impedancia de retorno, blindaje entre capas
- **NO cortar el ground plane** (cada corte crea una antena)
- PCB 4 capas estándar: Señales / GND / Power / Señales

**3. Separación analógico-digital**
- Zonas físicamente separadas en PCB
- AGND y DGND se unen en **un solo punto** (star ground, bajo el ADC)

**4. Blindaje**
- Cables blindados para señales sensibles
- En aviónica, blindaje conectado a estructura del avión

**5. Filtrado**
- Ferritas en líneas de alimentación (bloquean HF, pasan DC)
- Filtros RC/LC en entradas de señal
- Filtro anti-aliasing antes del ADC

**6. Layout de PCB**
- Trazas de clock cortas, lejos de conectores
- Impedancia controlada para señales >50 MHz
- Resistencias de terminación contra reflexiones

### Reducción de ruido en firmware

**Oversampling y promediado:**  
**ruido_reducido = ruido / √N**

N=16 muestras → ruido/4 → ≈ +2 bits de resolución efectiva

```c
#define NUM_SAMPLES 16
uint16_t adc_read_averaged(uint8_t channel) {
    uint32_t sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += ADC_Read(channel);
    }
    return (uint16_t)(sum / NUM_SAMPLES);
}
```

$Nota: Oversampling en firmware NO reemplaza el filtro anti-aliasing analógico. El aliasing ya ocurrió antes del ADC.

$Nota: En DO-178C, el filtrado en firmware debe documentarse, verificarse con tests, y analizar comportamiento ante fallas.

---

## Cadena completa de protección contra ruido

```
Blindaje (cables) → Filtro anti-aliasing → Desacoplo (100nF) → GND plane (layout) → ADC → Firmware (oversample)
```

---

## Fórmulas maestras del Módulo 2

### Diodos y transistores
| Fórmula | Descripción |
|---------|-------------|
| R = (V_GPIO − V_f) / I_f | Resistor limitador LED |
| I_B = I_C / β_forzado | Corriente base BJT (β_forzado=10) |
| R_B = (V_GPIO − 0.7) / I_B | Resistencia base BJT |
| P = I² × R_DS(on) | Pérdida potencia MOSFET |
| V_GS = V_Gate − V_Source | Voltaje gate-source |

### Op-amps
| Fórmula | Configuración |
|---------|---------------|
| V_out = −(R_f/R_in) × V_in | Inversor |
| V_out = (1 + R_f/R_in) × V_in | No-inversor |
| V_out = V_in | Buffer (ganancia=1) |
| V_out = (R_f/R1)(V2−V1) | Diferencial |
| f_c = 1/(2π × R_f × C) | Filtro pasa-bajos |

### ADC/DAC
| Fórmula | Descripción |
|---------|-------------|
| LSB = V_ref / 2^N | Resolución mínima |
| V = código × LSB | Código → voltaje |
| código = V_in × 2^N / V_ref | Voltaje → código |
| f_s ≥ 2 × f_max | Nyquist |
| SNR = 6.02N + 1.76 dB | SNR máximo de N bits |

### Reguladores
| Fórmula | Descripción |
|---------|-------------|
| P = (V_in − V_out) × I_out | Disipación LDO |
| η = V_out / V_in | Eficiencia LDO |
| V_out = D × V_in | Buck (D=duty cycle) |
| ruido_reducido = ruido/√N | Oversampling |

---

## Malentendidos comunes identificados

1. "Un diodo siempre cae 0.7V" → Depende del tipo (Schottky ~0.3V, LED 1.8-3.2V)
2. "Uso el β del datasheet para diseñar" → Usar β_forzado = 10 para garantizar saturación
3. "Saturación" significa lo mismo en BJT y MOSFET → Significan cosas **opuestas** como switch
4. "El filtro pasa-bajos filtra voltajes altos" → Filtra **frecuencias** altas, no voltajes
5. "Puedo arreglar aliasing en firmware" → Imposible, el daño es irreversible
6. "Switching es siempre mejor que LDO" → LDO es mejor para alimentar ADCs (bajo ruido)
7. "Código máximo del ADC = V_ref" → Es V_ref − 1 LSB
8. "Resolución = precisión en ADC" → ENOB < bits nominales por ruido
9. "El op-amp hace V+ = V−" → La realimentación negativa fuerza el equilibrio, no el op-amp solo

---

## Referencias bibliográficas

| Tema | Referencia | Capítulos |
|------|-----------|-----------|
| Semiconductores, transistores | Horowitz & Hill — The Art of Electronics | Cap. 1-3 |
| Semiconductores (teórico) | Sedra & Smith — Microelectronic Circuits | Cap. 3-5 |
| Op-amps | Horowitz & Hill | Cap. 4 |
| Op-amps (app notes) | Texas Instruments Application Notes | Varios |
| ADC/DAC | Horowitz & Hill | Cap. 13 |
| Reguladores | Horowitz & Hill | Cap. 9 |
| EMI/EMC | Henry Ott — Electromagnetic Compatibility Engineering | Todo |
| EMC aviación | DO-160G | Secciones 15-22 |
| Semiconductores (alternativa) | Razavi — Fundamentals of Microelectronics | Cap. 1-5 |

---

*Resumen generado al cierre del Módulo 2 — Mayo 2026*
