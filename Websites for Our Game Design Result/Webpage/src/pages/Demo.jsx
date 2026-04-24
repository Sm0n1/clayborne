import React, { useState } from 'react';
import './Demo.css';


function Demo() {
  const [active, setActive] = useState(1);

  return (
     <div className="section">
      <h2>Demo</h2>
      <div>Demo video here</div>
    </div>

  );
}

export default Demo;
