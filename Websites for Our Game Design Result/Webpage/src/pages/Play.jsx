import React from 'react';
import './Play.css';



function Play() {

  const controls = [
  { key: 'WASD', action: 'Move Left / right', desc: '' },
  { key: 'Space · Z · J', action: 'Jump', desc: '' },
  { key: 'Shift · X · K', action: 'Throw head', desc: '' },
];

  function handleLaunchGame() {
    window.open("YOUR_GAME_LINK_HERE", "_blank");
  }



    return (
    <div className="play-page">

      <div className="play-header">
        <p className="play-subtitle">Ready to play?</p>
        <h2 className="play-title">Clayborne</h2>
        <p className="play-desc">A puzzle platformer about losing your head....literally.</p>
      </div>

      <div className="play-preview">
        <p className="play-preview-text">ADD PICTER OF THE GAME</p>
      </div>

      <div className="play-launch">
        <button className="launch-btn" onClick={handleLaunchGame}>
          Launch Game
        </button>
        <p className="launch-note">Opens in a new tab — no install needed</p>
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
