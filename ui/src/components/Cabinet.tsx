import { useState } from "react";
import { colors, layout, typography } from "../theme/tokens";
import type { IrState } from "../bridge/nativeBridge";

interface CabinetProps {
  ir: IrState;
  onBrowseForIr: () => Promise<IrState>;
  error: string | null;
}

// Caixa + microfone + IR loader (FR-005/FR-006/FR-V02). Grelha/textura final depende de
// FR-V01/FR-V03 -- aqui e so a estrutura funcional.
export function Cabinet({ ir, onBrowseForIr, error }: CabinetProps) {
  const [busy, setBusy] = useState(false);

  const handleBrowse = async () => {
    setBusy(true);
    try {
      await onBrowseForIr();
    } finally {
      setBusy(false);
    }
  };

  return (
    <section
      style={{
        background: colors.panel,
        border: layout.borderMetal,
        borderRadius: layout.panelRadius,
        padding: "14px 18px",
        display: "flex",
        flexDirection: "column",
        gap: "8px",
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
        Caixa + Microfone (IR)
      </span>
      <div style={{ display: "flex", alignItems: "center", gap: "10px" }}>
        <span style={{ fontFamily: typography.mono, fontSize: "12px", color: colors.textPrimary }}>
          {ir.name} {ir.isDefault && "(padrao embutido)"}
        </span>
        <button
          onClick={handleBrowse}
          disabled={busy}
          style={{
            fontFamily: typography.body,
            fontSize: "12px",
            padding: "6px 10px",
            border: `1px solid ${colors.metalBrushed}`,
            background: colors.panelRaised,
            color: colors.brassBright,
            cursor: busy ? "wait" : "pointer",
          }}
        >
          {busy ? "Carregando..." : "Carregar IR..."}
        </button>
      </div>
      {error && <span style={{ color: colors.danger, fontSize: "12px" }}>{error}</span>}
    </section>
  );
}
