import { useEffect, useRef } from "react";
import runSprite from "../images/run.png";
import jumpSprite from "../images/jump.png";
import runBodySprite from "../images/run_body.png";
import fallBodySprite from "../images/fall_body.png";
 
export default function CharacterAnimation({
  runSrc      = runSprite,
  jumpSrc     = jumpSprite,
  runBodySrc  = runBodySprite,
  fallBodySrc = fallBodySprite,
  scale = 4,
}) {
  const canvasRef = useRef(null);
 
  const FRAME_W   = 8;
  const FRAME_H   = 16;
  const RUN_COLS  = 10;
  const RUN_TOTAL = 50;
  const charW     = FRAME_W * scale;
  const charH     = FRAME_H * scale;
 
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext("2d");
    ctx.imageSmoothingEnabled = false;
 
    canvas.width  = charW;
    canvas.height = charH;
 
    let runFrame = 0, runTick = 0;
    const runDelay = 3;
    let rafId;
 
    const imgs = { run: new Image() };
    let loaded = false;
 
    function tick() {
      ctx.clearRect(0, 0, charW, charH);
      runTick++;
      if (runTick >= runDelay) { runTick = 0; runFrame = (runFrame + 1) % RUN_TOTAL; }
      ctx.drawImage(
        imgs.run,
        (runFrame % RUN_COLS) * FRAME_W,
        Math.floor(runFrame / RUN_COLS) * FRAME_H,
        FRAME_W, FRAME_H,
        0, 0, charW, charH
      );
      rafId = requestAnimationFrame(tick);
    }
 
    imgs.run.onload = () => { loaded = true; tick(); };
    imgs.run.src = runSrc;
 
    return () => cancelAnimationFrame(rafId);
  }, [runSrc, scale, charW, charH]);
 
  return (
    <div style={{ position: "relative", display: "inline-block" }}>
      {/*<p className="tagline">Loose your head to find your way!</p>*/}
      <canvas
        ref={canvasRef}
        style={{
          position: "absolute",
          top: "50%",
          left: "50%",
          transform: "translate(1000%, 10%)",
          imageRendering: "pixelated",
          width: `${charW}px`,
          height: `${charH}px`,
          pointerEvents: "none",
        }}
      />
    </div>
  );
}
 