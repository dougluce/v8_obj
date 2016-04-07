'use strict'
/* global describe it before */
const binary = require('node-pre-gyp')
const path = require('path')
const v8_obj_path = binary.find(path.resolve(path.join(__dirname, '../package.json')))
const V8_Obj = require(v8_obj_path)
const expect = require('chai').expect

describe('v8_obj', function () {
  describe('Basic functions', function () {
    it('can set values', function () {
      const obj = new V8_Obj()
      obj['first'] = 'Value for first key'
      expect(obj['first']).to.equal('Value for first key')
      expect(obj['second']).to.be.undefined
    })
    it('can do for-of', function () {
      const obj = new V8_Obj()
      obj['first'] = 'A Value'
      obj['second'] = 'A Value'
      let i = 0
      for (let val of obj) {
        expect(val).to.equal('A Value')
        i++
      }
      expect(i).to.equal(2)
    })
  })
})
