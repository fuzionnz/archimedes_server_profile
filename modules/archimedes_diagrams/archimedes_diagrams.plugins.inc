<?php

class ArchimedesServerPlugin_dataset extends ArchimedesServerPlugin {
  public function parse($value) {
    return array(
      'title' => $value->getAttributeNS($this->ns, 'title'),
      'value' => $value->nodeValue,
    );  
  }
}
