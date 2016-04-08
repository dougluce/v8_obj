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
      expect(obj[Symbol.iterator]).to.be.a('function')
      let iter = obj[Symbol.iterator]()
      expect(iter.next).to.be.a('function')
      expect(iter.next()).to.deep.equal({ value: 'first', done: false })
      expect(iter.next()).to.deep.equal({ value: 'second', done: false })
      expect(iter.next()).to.deep.equal({ done: true })
      const seen = {}
      let i = 0
      for (let val of obj) {
        seen[val] = 'seen'
        i++
      }
      expect(i).to.equal(2)
      expect(seen).to.deep.equal({first: 'seen', second: 'seen'})
    })
  })
})
