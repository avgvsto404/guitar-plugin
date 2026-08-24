import { colors, layout, typography } from "../theme/tokens";

interface KnobProps {
  label: string;
  value: number;
  onChange: (value: number) => void;
  disabled?: boolean;
}

// Placeholder de controle (T033/T034): um slider rotulado. O knob visual real (giratorio,
// com textura de metal/latao) depende dos assets finais de FR-V03/FR-V04.
export function Knob({ label, value, onChange, disabled }: KnobProps) {
  return (
    <div
      style={{
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        gap: "4px",
        opacity: disabled ? 0.4 : 1,
        width: layout.knobSize,
      }}
    >
      <input
        type="range"
        min={0}
        max={1}
        step={0.01}
        value={value}
        disabled={disabled}
        onChange={(e) => onChange(Number(e.target.value))}
        style={{ writingMode: "vertical-lr", direction: "rtl", height: "72px", accentColor: colors.brassBright }}
        aria-label={label}
      />
      <span
        style={{
          fontFamily: typography.body,
          fontSize: "11px",
          letterSpacing: "0.05em",
          textTransform: "uppercase",
          color: colors.textMuted,
        }}
      >
        {label}
      </span>
    </div>
  );
}
