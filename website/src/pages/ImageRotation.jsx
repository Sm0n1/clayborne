import React, { useState } from "react";
import "./ImageRotation.css";

function ImageRotation({ images }) {
    const [current, setCurrent] = useState(0);

    function prev() {
        setCurrent(current === 0 ? images.length - 1 : current - 1);
    }

    function next() {
        setCurrent(current === images.length - 1 ? 0 : current + 1);
    }

    return (
        <div className="carousel">
            <button className="carousel-btn left" onClick={prev}>‹</button>

            <div className="carousel-window">
                <img src={images[current]} alt={`screenshot ${current + 1}`} />
            </div>

            <button className="carousel-btn right" onClick={next}>›</button>

            <div className="carousel-dots">
                {images.map((_, i) => (
                    <span
                        key={i}
                        className={i === current ? "dot active" : "dot"}
                        onClick={() => setCurrent(i)}
                    />
                ))}
            </div>
        </div>
    );
}

export default ImageRotation;