import React from "react";
import "./Home.css";
import { useState } from "react";
import About from "./About.jsx"; 
import Demo from "./Demo.jsx"; 
import Play from "./Play.jsx"; 

function Home() {

  const [activeTab, setActiveTab] = useState(0);
  const team = [
  { name: "Pontus Andersson", role: "", img: "image" },
  { name: "Arindam Kishore Dutta", role: "", img: "image" },
  { name: "Elin Eriksson", role: "", img: "image" },
  { name: "Simon Jutvreten", role: "", img: "image" },
  { name: "Erik Olsson", role: "", img: "image" }
];

  return (
    <div className="home-container">

      {/* LOGO */}
      <section className="hero">
        <h1 className="game-title">CLAYBORNE</h1>
        <img src="character.png" alt=""></img>
        <p className="tagline">Loose your head to find your way!</p>

        <div className="hero-buttons">
      {/* NAV TABS */}
      <nav className="nav-tabs">
        <button className={activeTab === 0 ? "tab active" : "tab"} onClick={() => setActiveTab(0)}>Home</button>
        <button className={activeTab === 1 ? "tab active" : "tab"} onClick={() => setActiveTab(1)}>About</button>
        <button className={activeTab === 2 ? "tab active" : "tab"} onClick={() => setActiveTab(2)}>Demo</button>
        <button className={activeTab === 3 ? "tab active" : "tab"} onClick={() => setActiveTab(3)}>Play</button>
      </nav>
        </div>
      </section>


      {/* CONTENT */}
      <section className="content">

  {activeTab === 0 && (
    <div className="section">
      <h2>Welcome</h2>
      <p>Welcome to Clayborne, our game project.</p>
        {/* TEAM*/}
        <div className="section">
          <h2>The Team Of Clayborne</h2>
          <div className="team-grid">
            {team.map((person, index) => (
              <div className="profile-card" key={index}>
                <img src={person.img} alt={person.name} />
                <h3>{person.name}</h3>
                <p>{person.role}</p>
              </div>
            ))}
          </div>
        </div>
    </div>
  )}

  {activeTab === 1 && <About />}

  {activeTab === 2 && <Demo />}

  {activeTab === 3 && <Play/> }
  
</section>
    </div>
  );
}

export default Home;