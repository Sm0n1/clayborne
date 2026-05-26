import './Play.css';
import bgImg from "../main-menu/Mainmenu_BG.png";
import fgImg from "../main-menu/Mainmenu_FG.png";
import textImg from "../main-menu/Mainmenu_text.png";
import React, { useRef, useState } from 'react';
import { initGame } from '../gameLoader';
import './Play.css';

function Play() {

  const controls = [
  { key: 'W·A·S·D', action: 'Move Left / right', desc: '' },
  { key: 'Space · Z · J', action: 'Jump', desc: '' },
  { key: 'Shift · X · K', action: 'Throw head', desc: '' },
];

const canvasRef = useRef(null);

    const [started, setStarted] = useState(false);
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState(null);

    async function handleStart() {
        if (started || loading) {
            return;
        }
        setLoading(true);
        setError(null);
        try {
            await initGame(canvasRef.current);
            setStarted(true);
        } catch (e) {
            setError(e);
        } finally {
            setLoading(false);
        }
    }


  function handleLaunchGame() {
    window.open("https://sm0n1.github.io/clayborne/game.html", "_blank");
  }



    return (
  <div className="play-page">
    <div className="play-header">
      <p className="play-subtitle">Ready to play the game demo?</p>

      <h2 className="play-title">Clayborne</h2>

      <p className="play-desc">
        A puzzle platformer about losing your head....literally.
      </p>
    </div>

    <div className="play-preview">
      <img className="game-bg" src={bgImg} alt="Game background" />

      <img className="game-fg" src={fgImg} alt="Game foreground" />

      <img className="game-text" src={textImg} alt="Clayborne logo" />
    </div>

    <div className="play-launch">
  <button
    className="launch-btn"
    onClick={handleStart}
    disabled={loading}
  >
    {loading ? "Loading..." : "Launch Game"}
  </button>

  <p className="launch-note">
    Play instantly in your browser
  </p>

  {error && <p>{String(error.message || error)}</p>}

  <canvas
    ref={canvasRef}
    width={800}
    height={600}
    style={{ display: started ? "block" : "none" }}
  />
</div>
          
      
    <div className="controls-section">
      <p className="launch-note">Controls</p>

      <div className="badge-card">
        <div className="controls-grid">
          {controls.map(({ key, action, desc }) => (
            <div className="control-card" key={action}>
              <span className="control-key">{key}</span>

              <p className="control-action">{action}</p>

              <p className="control-desc">{desc}</p>
            </div>
          ))}
        </div>
      </div>
    </div>
  </div>
);

}

export default Play;

