import { useEffect, useState } from "react";
import { AmpHead } from "./components/AmpHead";
import { TubeScreamerPedal } from "./components/TubeScreamerPedal";
import { Cabinet } from "./components/Cabinet";
import { nativeBridge, type IrState, type PluginParams } from "./bridge/nativeBridge";
import { colors, typography } from "./theme/tokens";

const INITIAL_IR: IrState = { loaded: true, name: "carregando...", isDefault: true };

export default function App() {
  const [params, setParams] = useState<PluginParams | null>(null);
  const [ir, setIr] = useState<IrState>(INITIAL_IR);
  const [irError, setIrError] = useState<string | null>(null);

  useEffect(() => {
    nativeBridge.getState().then(setParams);
    const unsubParams = nativeBridge.onParamChanged((partial) =>
      setParams((prev) => (prev ? { ...prev, ...partial } : prev)),
    );
    const unsubIr = nativeBridge.onIrChanged(setIr);
    return () => {
      unsubParams();
      unsubIr();
    };
  }, []);

  if (!params) {
    return (
      <div style={{ color: colors.textMuted, fontFamily: typography.body, padding: "20px" }}>
        Conectando ao motor de audio...
      </div>
    );
  }

  const setParam = <K extends keyof PluginParams>(id: K, value: PluginParams[K]) => {
    setParams((prev) => (prev ? { ...prev, [id]: value } : prev));
    nativeBridge.setParam(id, value);
  };

  const handleBrowseForIr = async () => {
    setIrError(null);
    try {
      const result = await nativeBridge.browseForIr();
      setIr(result);
      return result;
    } catch {
      setIrError("Nao foi possivel carregar esse IR -- mantendo o cabinet padrao.");
      return ir;
    }
  };

  return (
    <div
      style={{
        background: colors.background,
        minHeight: "100vh",
        padding: "20px",
        display: "flex",
        flexDirection: "column",
        gap: "16px",
        fontFamily: typography.body,
      }}
    >
      <AmpHead
        params={params}
        mode={params.ampMode}
        onModeChange={(mode) => setParam("ampMode", mode)}
        onParamChange={(id, value) => setParam(id, value)}
      />
      <div style={{ display: "flex", gap: "16px", flexWrap: "wrap" }}>
        <TubeScreamerPedal
          params={params}
          bypass={params.tsBypass}
          onParamChange={(id, value) => setParam(id, value)}
          onBypassToggle={() => setParam("tsBypass", !params.tsBypass)}
        />
        <Cabinet ir={ir} onBrowseForIr={handleBrowseForIr} error={irError} />
      </div>
    </div>
  );
}
