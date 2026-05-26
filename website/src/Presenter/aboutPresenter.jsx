import { observer } from "mobx-react-lite";
import About from "/src/views/About.jsx";

const AboutPresenter = observer(function AboutPresenter(props) {

  function handleClickOnAbout() {
    props.setActiveTab(1); 
  }

  return (
    <About clickOnAbout={handleClickOnAbout} />
  );
});

export { AboutPresenter };