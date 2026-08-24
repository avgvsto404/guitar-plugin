#!/usr/bin/env python3
"""Gera um Impulse Response PLACEHOLDER (ruido filtrado com decaimento exponencial) so para
o scaffold compilar e o CabinetLoader ter algo para carregar durante o desenvolvimento.

Isso NAO substitui T090 (specs/001-nikolayevsk-dark-dsp/tasks.md): um IR de cabinet real,
capturado ou com licenca compativel, precisa entrar antes de qualquer release publico.
"""

import math
import random
import struct
import wave

SAMPLE_RATE = 44100
DURATION_S = 0.15
OUT_PATH = "assets/ir/default_cab_PLACEHOLDER.wav"


def generate_samples():
    n = int(SAMPLE_RATE * DURATION_S)
    samples = []
    lowpass_state = 0.0
    alpha = 0.25  # one-pole lowpass, corta agudos pra parecer menos "ruido cru"
    for i in range(n):
        white = random.uniform(-1.0, 1.0)
        lowpass_state += alpha * (white - lowpass_state)
        envelope = math.exp(-6.0 * (i / n))
        samples.append(lowpass_state * envelope)
    peak = max(abs(s) for s in samples) or 1.0
    return [s / peak * 0.9 for s in samples]


def write_wav(path, samples):
    with wave.open(path, "wb") as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(SAMPLE_RATE)
        frames = b"".join(struct.pack("<h", int(s * 32767)) for s in samples)
        f.writeframes(frames)


if __name__ == "__main__":
    write_wav(OUT_PATH, generate_samples())
    print(f"IR placeholder escrito em {OUT_PATH}")
