import React, { useRef, useState } from 'react';
import { initGame } from '../gameLoader';
import './Play.css';

function Play() {
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

    const controls = [
        { key: 'WASD', action: 'Move', desc: '' },
        { key: 'Space · Z · J', action: 'Jump', desc: '' },
        { key: 'Shift · X · K', action: 'Use Head', desc: '' },
        { key: 'F11', action: 'Toggle Fullscreen', desc: '' },
    ];

    return (
        <div className="play-page">

        <div className="play-header">
            <p className="play-subtitle">
                Ready to play?
            </p>

            <h2 className="play-title">
                Clayborne
            </h2>

            <p className="play-desc">
                A puzzle platformer about losing your head....literally.
            </p>
        </div>

        <div className="play-launch">
            {!started && (
                <div>
                    <button
                        className="launch-btn"
                        onClick={handleStart}
                        disabled={loading}
                    >
                        {loading ? "Loading..." : "Launch Game"}
                    </button>

                    {error && <p>Failed to load game</p>}
                </div>
            )}

            <canvas
                ref={canvasRef}
                className="game-canvas"
                tabIndex={0}
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
