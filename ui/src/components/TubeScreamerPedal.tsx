import { colors, layout, typography } from "../theme/tokens";
import { Knob } from "./Knob";
import type { PluginParams } from "../bridge/nativeBridge";

type PedalParamKey = "tsDrive" | "tsTone" | "tsLevel";

interface TubeScreamerPedalProps {
  params: Pick<PluginParams, PedalParamKey>;
  bypass: boolean;
  onParamChange: (id: PedalParamKey, value: number) => void;
  onBypassToggle: () => void;
}

const KNOBS: { id: PedalParamKey; label: string }[] = [
  { id: "tsDrive", label: "Drive" },
  { id: "tsTone", label: "Tone" },
  { id: "tsLevel", label: "Level" },
];

// Pedal overdrive (FR-004/FR-010). Bypass e hard bypass no DSP (research.md secao 2) --
// aqui e so o estado visual do footswitch.
export function TubeScreamerPedal({ params, bypass, onParamChange, onBypassToggle }: TubeScreamerPedalProps) {
  return (
    <section
      style={{
        background: colors.panelRaised,
        border: layout.borderMetal,
        borderRadius: layout.panelRadius,
        padding: "12px 16px",
        display: "flex",
        alignItems: "center",
        gap: "16px",
        width: "fit-content",
      }}
    >
      <span
        style={{
          fontFamily: typography.heading,
          fontSize: "12px",
          letterSpacing: "0.08em",
          color: colors.textMuted,
        }}
      >
        Overdrive
      </span>
      <div style={{ display: "flex", gap: "10px" }}>
        {KNOBS.map(({ id, label }) => (
          <Knob
            key={id}
            label={label}
            value={params[id]}
            disabled={bypass}
            onChange={(v) => onParamChange(id, v)}
          />
        ))}
      </div>
      <button
        onClick={onBypassToggle}
        aria-pressed={!bypass}
        title={bypass ? "Pedal desligado (bypass)" : "Pedal ligado"}
        style={{
          width: "28px",
          height: "28px",
          borderRadius: "50%",
          border: `2px solid ${colors.metalBrushed}`,
          background: bypass ? colors.ledOff : colors.ledOn,
          cursor: "pointer",
        }}
      />
    </section>
  );
}
