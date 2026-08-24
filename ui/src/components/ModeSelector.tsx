import { colors, typography } from "../theme/tokens";
import type { AmpMode } from "../bridge/nativeBridge";

const MODES: { id: AmpMode; label: string }[] = [
  { id: "clean", label: "Clean" },
  { id: "rhythm", label: "Rhythm" },
  { id: "lead", label: "Lead" },
];

interface ModeSelectorProps {
  mode: AmpMode;
  onChange: (mode: AmpMode) => void;
}

// FR-001/FR-002/FR-010. Labels sao os mesmos usados publicamente ("Clean"/"Rhythm"/"Lead") --
// os identificadores internos de voicing (BRIT_HI/CALI_HI, ver plan.md) nunca aparecem aqui.
export function ModeSelector({ mode, onChange }: ModeSelectorProps) {
  return (
    <div style={{ display: "flex", gap: "2px" }}>
      {MODES.map(({ id, label }) => {
        const active = id === mode;
        return (
          <button
            key={id}
            onClick={() => onChange(id)}
            style={{
              fontFamily: typography.heading,
              fontSize: "13px",
              letterSpacing: "0.08em",
              padding: "8px 14px",
              border: `1px solid ${colors.metalBrushed}`,
              background: active ? colors.wineBright : colors.panelRaised,
              color: active ? colors.textPrimary : colors.textMuted,
              cursor: "pointer",
            }}
          >
            {label}
          </button>
        );
      })}
    </div>
  );
}
