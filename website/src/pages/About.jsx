import React from 'react';
import './About.css';
import githubImg from "../images/github.png";
import docsImg from "../images/book-open.png";

import ImageRotation from "./ImageRotation.jsx";
import screenshot1 from "../trailer images/screenshot1.png";
import screenshot2 from "../trailer images/screenshot2.png";
import screenshot3 from "../trailer images/screenshot3.png";
import sparkles from "../images/sparkles.png";
import eyeClosed from "../images/eye-closed.png";
import mapPlus from "../images/map-plus.png";
import astroid from "../images/astroid.png";



function About() {

   const team = [
{ name: "Pontus Andersson", role: "Programmer, Storyteller", img: "image" },
{ name: "Arindam Kishore Dutta", role: "Animator, Designer, Storyteller", img: "image" },
{ name: "Elin Eriksson", role: "Tileset Artist, Storyteller, Web Developer", img: "image" },
{ name: "Simon Jutvreten", role: "Lead Programmer, Game Designer, Sound Designer, Music Composer", img: "image" },
{ name: "Erik Olsson", role: "Programmer, Level Designer", img: "image" },
];

const mechanics = [
  {
    icon: sparkles, title: "Detach & Explode Your Head", desc: "Throw your head to reach new areas, then explode it to send nearby objects, and yourself, flying."
  },
  {
    icon: eyeClosed, title: "Knowledge-Gated Exploration", desc: "The world is open from the start. Progress by discovering hidden mechanics through experimentation, not by unlocking abilities."
  },
  {
    icon: mapPlus, title: "Five Realms to Explore", desc: "Travel through the five realms, each with unique environments, hazards, and puzzle."
  },
  {
    icon: astroid, title: "Soul Pentagons", desc: "Find five scattered Soul Pentagons to recover lost memories and uncover the truth behind Kemerheim's disappearance."
  },
];

function handleGDDC() {
    window.open("GOOGLE DRIVE LINK", "_blank");
  }

  function handleGitHub() {
    window.open("https://github.com/Sm0n1/clayborne");
  }


  return (
    <div>
      <div className = "pageTitle">
         <p>
        Clayborne is a 2D puzzle-platformer set in the forgotten world of Kemerheim.
      </p>
      <p>
        Explore ancient ruins, solve environmental puzzles,
        and uncover the truth behind a lost civilization of clay and crystal.
      </p>
        
        </div>
            <div className="play-badges">
        <div className="badge-card">
          <p className="badge-title">Puzzle platformer</p>
          <p className="badge-desc">Clever level design</p>
        </div>
        <div className="badge-card">
          <p className="badge-title">Browser-based</p>
          <p className="badge-desc">No download needed</p>
        </div>
        <div className="badge-card">
          <p className="badge-title">Pixel art</p>
          <p className="badge-desc">Hand-crafted style</p>
        </div>
      </div>
      <ImageRotation images={[screenshot1, screenshot2, screenshot3]} />

      <div className="mechanics-section">
        <h2 className="pageTitel">Game Mechanics</h2>
        <div className="mechanics-grid">
          {mechanics.map(({ icon, title, desc }) => (
            <div className="mechanic-card" key={title}>
              <img
  src={icon}
  alt={title}
  className="mechanic-icon"
/>
              <h3 className="mechanic-title">{title}</h3>
              <p className="mechanic-desc">{desc}</p>
            </div>
          ))}
        </div>
      </div>



        {/* GDDC  + GIT*/}
        <div className="card-container">

          <div className="info-card" onClick={handleGDDC}>
            <h2 className="pageTitel">Documentation</h2>
            <p>Read the detailed Game Development Document here</p>
             <img style={{ display: "block", margin: "0 auto" }} src={docsImg} alt="Documentation" />
          </div>

          <div className="info-card" onClick={handleGitHub}>
            <h2 className="pageTitel">Code</h2>

            <p>
              Explore our GitHub repository and source code.
            </p>

            <span className="card-link">
              <img style={{ display: "block", margin: "0 auto" }} src={githubImg} alt="GitHub" />
            </span>

          </div>

        </div>
       

      </div>

  );
}

export default About;
