import React from "react";
import "./Home.css";
import { useState } from "react";
import About from "./About.jsx"; 
import Demo from "./Demo.jsx"; 
import Play from "./Play.jsx"; 
import CharacterAnimation from "./CharacterAnimation.jsx";
import runSprite from "../images/run.png";
import jumpSprite from "../images/jump.png";
import runBodySprite from "../images/run_body.png";
import fallBodySprite from "../images//fall_body.png"


function Home() {

  const [activeTab, setActiveTab] = useState(0);

     const team = [
{ name: "Pontus Andersson", role: "Programmer, Storyteller", img: "image" },
{ name: "Arindam Kishore Dutta", role: "Animator, Designer, Storyteller", img: "image" },
{ name: "Elin Eriksson", role: "Tileset Artist, Storyteller, Web Developer", img: "image" },
{ name: "Simon Jutvreten", role: "Programmer", img: "image" },
{ name: "Erik Olsson", role: "Programmer, Level Designer", img: "image" },
];

  return (
    <div className="home-container">

      {/* LOGO */}
      <section className="hero">
        <h1 className="game-title">CLAYBORNE</h1>
        <p className="tagline">Loose your head to find your way!</p>
       
        {/* CHARACTER RUNNER — sits just below the tagline, jumps up on it 
         <CharacterAnimation scale={4} />*/}
        <div className="hero-buttons">
      {/* NAV TABS */}
      <nav className="nav-tabs">
        <button className={activeTab === 0 ? "tab active" : "tab"} onClick={() => setActiveTab(0)}>Home</button>
        <button className={activeTab === 1 ? "tab active" : "tab"} onClick={() => setActiveTab(1)}>The Game</button>
        {/*<button className={activeTab === 2 ? "tab active" : "tab"} onClick={() => setActiveTab(2)}>Demo</button>*/}
        <button className={activeTab === 3 ? "tab active" : "tab"} onClick={() => setActiveTab(3)}>Play</button>
      </nav>
        </div>
      </section>


      {/* CONTENT */}
      <section className="content">

  {activeTab === 0 && (
    <div>
      {/*<h2 className = "pageTitle">Welcome</h2>*/}
      <div className = "pageTitel">

         {/* STORY */}
       <div className = "pageTitle">
          <h2 className="pageTitel">The Story</h2>
           <div className="story-text">
              <p>
                Long ago, the world of Kemerheim flourished through the power of clay,
                crystal, and ancient magic.
              </p>

              <p>
                In search of immortality, humanity transferred their souls into
                artificial bodies known as Golems.
              </p>

              <p>
                But the process was flawed. Memories faded, emotions disappeared,
                and the people of Kemerheim slowly became hollow shells.
              </p>

              <p>
                You awaken as a forgotten Golem, unfinished and without memories,
                searching the ruins of a dead world for the truth about who you are.
              </p>
            </div>
        </div>
        
                       {/* TEAM*/}
        <div className="pageTitle">
          <h2 className = "pageTitle">The Team Of Clayborne</h2>
          <div className="team-grid">
            {team.map((person, index) => (
              <div className="profile-card" key={index}>
                <img src={person.img} alt={person.name} />
                <p>{person.name}</p>
                <p className="member-role">{person.role}</p>
              </div>
            ))}
          </div>
        </div>
        
        </div>
     
    </div>
  )}

  {activeTab === 1 && <About />}

  {/*activeTab === 2 && <Demo />*/}

  {activeTab === 3 && <Play/> }
  
</section>
    </div>
  );
}

export default Home;