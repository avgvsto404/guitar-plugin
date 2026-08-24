# Nikolayevsk DARK DSP

Plugin de guitarra VST3, **gratuito e open source (GPLv3)**, signature do
[Nikolayevsk](https://github.com/nikolayevsk404) — guitarrista brasileiro de progressive power
metal. Fusão de voicing entre um amp britânico de alto gain e um amp americano de gain em
cascata, com pedal overdrive, cabinet/IR loader e identidade visual gótica/rústica.

> Projeto em fase de especificação (Spec-Driven Development). Nenhum código de DSP/plugin foi
> escrito ainda — a implementação segue os documentos em `specs/001-nikolayevsk-dark-dsp/`.

## Cadeia de sinal

```
Guitarra → Pedal (Tube Screamer-like) → Cabeçote (Clean | Rhythm | Lead) → Caixa + Mic (IR) → DAW
```

## Documentação (Spec-Driven Development)

Todo o desenvolvimento parte de specs versionadas antes do código:

- [`spec.md`](specs/001-nikolayevsk-dark-dsp/spec.md) — o quê e por quê (requisitos,
  critérios de aceite, voicing target, identidade visual).
- [`plan.md`](specs/001-nikolayevsk-dark-dsp/plan.md) — como (arquitetura, stack, estrutura
  de projeto).
- [`research.md`](specs/001-nikolayevsk-dark-dsp/research.md) — racional técnico por trás de
  cada decisão de DSP e de integração UI↔áudio.
- [`tasks.md`](specs/001-nikolayevsk-dark-dsp/tasks.md) — quebra executável de tarefas.

## Stack

- **DSP / host de plugin**: C++20, [JUCE](https://juce.com/) 8.x, CMake
- **UI**: React 18 + TypeScript (Vite), renderizada dentro do `WebBrowserComponent` do JUCE
- **Formato**: VST3 (Windows 10+ e macOS 12+)
- **Testes**: Catch2 (DSP), `pluginval` (validação de plugin), GitHub Actions (CI multiplataforma)

## Licença

[GPLv3](LICENSE). Uso, modificação e redistribuição livres, inclusive comercial, desde que o
código-fonte de qualquer versão distribuída permaneça aberto sob a mesma licença.

## Identidade visual

Direção de arte por Haruna Lamberti. Estética gótica/rústica alinhada a prog/power metal —
detalhes em [`spec.md`](specs/001-nikolayevsk-dark-dsp/spec.md#identidade-visual-mandatory).

## Build

Ainda não aplicável — sem código-fonte de plugin criado. Instruções de build serão
adicionadas junto com a Fase 0 de `tasks.md`.
