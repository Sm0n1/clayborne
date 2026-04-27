import React, { useState } from 'react';
import './Demo.css';


function Demo() {
  const [active, setActive] = useState(1);

  return (
     <div className = "pageTitle">
      <h2 className = "about-text">Demo</h2>
      <div className = "about-text">Demo video here</div>
    </div>

  );
}

export default Demo;
