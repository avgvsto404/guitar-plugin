import { colors, layout, typography } from "../theme/tokens";
import { Knob } from "./Knob";
import { ModeSelector } from "./ModeSelector";
import type { AmpMode, PluginParams } from "../bridge/nativeBridge";

type AmpParamKey = "gain" | "bass" | "mid" | "treble" | "presence" | "master";

interface AmpHeadProps {
  params: Pick<PluginParams, AmpParamKey>;
  mode: AmpMode;
  onModeChange: (mode: AmpMode) => void;
  onParamChange: (id: AmpParamKey, value: number) => void;
}

const KNOBS: { id: AmpParamKey; label: string }[] = [
  { id: "gain", label: "Gain" },
  { id: "bass", label: "Bass" },
  { id: "mid", label: "Mid" },
  { id: "treble", label: "Treble" },
  { id: "presence", label: "Presence" },
  { id: "master", label: "Master" },
];

// Cabecote (FR-007/FR-010) -- placeholder estrutural, sem a textura/moldura final (FR-V01/V02).
export function AmpHead({ params, mode, onModeChange, onParamChange }: AmpHeadProps) {
  return (
    <section
      style={{
        background: colors.panel,
        border: layout.borderMetal,
        borderRadius: layout.panelRadius,
        padding: "16px 20px",
        display: "flex",
        flexDirection: "column",
        gap: "14px",
      }}
    >
      <header style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
        <h2
          style={{
            fontFamily: typography.heading,
            color: colors.brassBright,
            fontSize: "16px",
            letterSpacing: "0.1em",
            margin: 0,
          }}
        >
          Nikolayevsk DARK DSP
        </h2>
        <ModeSelector mode={mode} onChange={onModeChange} />
      </header>
      <div style={{ display: "flex", gap: "12px", justifyContent: "space-between" }}>
        {KNOBS.map(({ id, label }) => (
          <Knob key={id} label={label} value={params[id]} onChange={(v) => onParamChange(id, v)} />
        ))}
      </div>
    </section>
  );
}
