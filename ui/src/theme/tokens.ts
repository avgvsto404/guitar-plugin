// Tokens visuais placeholder (T031). Paleta e tipografia seguem FR-V01 (gotico/rustico,
// prog/power metal) mas os valores exatos e qualquer asset de logo/textura final DEVEM vir
// da direcao de arte da Haruna Lamberti (FR-V03) antes de qualquer release publico.
// Nada aqui deve ser tratado como identidade visual definitiva.

export const colors = {
  background: "#100c0e",
  panel: "#1c1416",
  panelRaised: "#241a1c",
  metalBrushed: "#3a3436",
  wine: "#5c1521",
  wineBright: "#8a1f30",
  brass: "#9c7b3f",
  brassBright: "#c7a15c",
  textPrimary: "#e8e1d8",
  textMuted: "#948b83",
  ledOff: "#3a1418",
  ledOn: "#e0384a",
  danger: "#e0384a",
} as const;

export const typography = {
  heading: '"Cinzel", "Times New Roman", serif',
  body: '"Inter", "Segoe UI", sans-serif',
  mono: '"JetBrains Mono", monospace',
} as const;

export const layout = {
  panelRadius: "6px",
  knobSize: "56px",
  borderMetal: `1px solid ${colors.metalBrushed}`,
} as const;
