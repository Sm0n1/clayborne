import React from 'react';
import './About.css';

function About() {

  function handleGDDC() {
    window.open("GOOGLE DRIVE LINK", "_blank");
  }

  function handleGitHub() {
    window.open("GITHUB LINK", "_blank");
  }

  return (
    <main className="page about-page">
      <div className="about-container">

        {/* STORY */}
        <section className="about-section">
          <h2>THE STORY</h2>
          <div className="about-text pixel-box">
            <p>Clayborne began as a game design course project.</p>
            <p>In Clayborne you play as a small clay figure.</p>
            <p>
              The game is built in a 2D pixel-art style with a focus on clever, satisfying puzzle design.
            </p>
          </div>
        </section>

        {/* GDDC */}
        <section className="about-section">
          <h2>DOCUMENTATION</h2>
          <p>Read the detailed Game Development Document here:</p>
          <button className="btn-pixel" onClick={handleGDDC}>
            GDDC
          </button>
        </section>

        {/* GITHUB */}
        <section className="about-section">
          <h2>CODE</h2>
          <p>To see our code, have a look at our GitHub:</p>
          <button className="btn-pixel btn-cyan" onClick={handleGitHub}>
            GitHub
          </button>
        </section>

      </div>
    </main>
  );
}

export default About;
